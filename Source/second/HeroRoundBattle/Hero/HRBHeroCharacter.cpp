#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

AHRBHeroCharacter::AHRBHeroCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCapsuleComponent()->SetCapsuleRadius(40.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.f);

	HeroMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeroMesh"));
	HeroMesh->SetupAttachment(GetRootComponent());
	HeroMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	HeroMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		HeroMesh->SetStaticMesh(CylinderMesh.Object);
		HeroMesh->SetWorldScale3D(FVector(0.8f, 0.8f, 1.8f));
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;

	AutoPossessAI = EAutoPossessAI::Disabled;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
}

void AHRBHeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHRBHeroCharacter, OwningPlayerIndex);
	DOREPLIFETIME(AHRBHeroCharacter, HeroIndex);
	DOREPLIFETIME(AHRBHeroCharacter, HeroLevel);
	DOREPLIFETIME(AHRBHeroCharacter, HeroDefinition);
	DOREPLIFETIME(AHRBHeroCharacter, EffectiveStats);
	DOREPLIFETIME(AHRBHeroCharacter, CurrentHP);
	DOREPLIFETIME(AHRBHeroCharacter, bIsDead);
	DOREPLIFETIME(AHRBHeroCharacter, HeroColor);
}

void AHRBHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisuals();
}

void AHRBHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !bIsDead)
	{
		TickCombat(DeltaTime);
	}
}

void AHRBHeroCharacter::InitHero(const FHeroDefinition& InDef, int32 InOwningPlayer, int32 InHeroIndex)
{
	HeroDefinition = InDef;
	OwningPlayerIndex = InOwningPlayer;
	HeroIndex = InHeroIndex;
	HeroLevel = 1;
	HeroColor = InDef.HeroColor;

	EffectiveStats = InDef.BaseStats;
	CurrentHP = EffectiveStats.MaxHP;

	GetCharacterMovement()->MaxWalkSpeed = EffectiveStats.MoveSpeed;

	UpdateVisuals();
}

void AHRBHeroCharacter::ApplyLevelUp(int32 NewLevel)
{
	HeroLevel = NewLevel;

	const FHeroStats& Base = HeroDefinition.BaseStats;
	int32 LevelsGained = NewLevel - 1;

	EffectiveStats.MaxHP = Base.MaxHP + Base.HPPerLevel * LevelsGained;
	EffectiveStats.Attack = Base.Attack + Base.AttackPerLevel * LevelsGained;
	EffectiveStats.Defense = Base.Defense + Base.DefensePerLevel * LevelsGained;

	// Full HP recovery on level up
	CurrentHP = EffectiveStats.MaxHP;

	OnHPChanged.Broadcast(CurrentHP, EffectiveStats.MaxHP);
}

void AHRBHeroCharacter::ResetForNewRound()
{
	bIsDead = false;
	CurrentHP = EffectiveStats.MaxHP;
	AttackTarget = nullptr;
	bHasMoveCommand = false;
	AttackCooldownRemaining = 0.f;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	OnHPChanged.Broadcast(CurrentHP, EffectiveStats.MaxHP);
}

// --- Commands ---

void AHRBHeroCharacter::CommandMoveTo(const FVector& Destination)
{
	if (bIsDead) return;

	AttackTarget = nullptr;
	MoveDestination = Destination;
	bHasMoveCommand = true;

	// Use simple MoveTo via CharacterMovement
	// Navigate to destination
	FVector Dir = (Destination - GetActorLocation()).GetSafeNormal2D();
	GetCharacterMovement()->RequestDirectMove(Dir * GetCharacterMovement()->MaxWalkSpeed, false);
}

void AHRBHeroCharacter::CommandAttack(AHRBHeroCharacter* Target)
{
	if (bIsDead || !Target || Target->IsDead()) return;

	AttackTarget = Target;
	bHasMoveCommand = false;
}

void AHRBHeroCharacter::CommandStop()
{
	AttackTarget = nullptr;
	bHasMoveCommand = false;
	GetCharacterMovement()->StopActiveMovement();
}

// --- Combat ---

void AHRBHeroCharacter::TickCombat(float DeltaTime)
{
	AttackCooldownRemaining = FMath::Max(0.f, AttackCooldownRemaining - DeltaTime);

	// Move command
	if (bHasMoveCommand)
	{
		float DistToTarget = FVector::Dist2D(GetActorLocation(), MoveDestination);
		if (DistToTarget < 50.f)
		{
			bHasMoveCommand = false;
			GetCharacterMovement()->StopActiveMovement();
		}
		else
		{
			FVector Dir = (MoveDestination - GetActorLocation()).GetSafeNormal2D();
			AddMovementInput(Dir, 1.f);
		}
		return;
	}

	// Attack target
	if (AttackTarget.IsValid() && !AttackTarget->IsDead())
	{
		float Dist = FVector::Dist(GetActorLocation(), AttackTarget->GetActorLocation());

		if (Dist <= EffectiveStats.AttackRange)
		{
			// In range -> attack
			GetCharacterMovement()->StopActiveMovement();
			if (AttackCooldownRemaining <= 0.f)
			{
				PerformAttack();
			}
		}
		else
		{
			// Move toward target
			FVector Dir = (AttackTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			AddMovementInput(Dir, 1.f);
		}
	}
	else
	{
		// No target - auto acquire nearest enemy
		AttackTarget = nullptr;
		float BestDist = TNumericLimits<float>::Max();

		for (TActorIterator<AHRBHeroCharacter> It(GetWorld()); It; ++It)
		{
			AHRBHeroCharacter* Other = *It;
			if (Other == this || Other->IsDead() || Other->GetOwningPlayer() == OwningPlayerIndex)
				continue;

			float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
			if (Dist < BestDist)
			{
				BestDist = Dist;
				AttackTarget = Other;
			}
		}

		if (AttackTarget.IsValid())
		{
			FVector Dir = (AttackTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
			AddMovementInput(Dir, 1.f);
		}
	}
}

void AHRBHeroCharacter::PerformAttack()
{
	if (!AttackTarget.IsValid() || AttackTarget->IsDead()) return;

	float CooldownTime = (EffectiveStats.AttackSpeed > 0.f) ? (1.f / EffectiveStats.AttackSpeed) : 1.f;
	AttackCooldownRemaining = CooldownTime;

	AttackTarget->ReceiveDamage(EffectiveStats.Attack, this);

	Multicast_PlayHitEffect();
}

void AHRBHeroCharacter::ReceiveDamage(float RawDamage, AHRBHeroCharacter* Attacker)
{
	if (!HasAuthority() || bIsDead) return;

	float ActualDamage = FMath::Max(1.f, RawDamage - EffectiveStats.Defense);
	CurrentHP = FMath::Max(0.f, CurrentHP - ActualDamage);

	OnHPChanged.Broadcast(CurrentHP, EffectiveStats.MaxHP);

	if (CurrentHP <= 0.f)
	{
		Die();
	}
}

void AHRBHeroCharacter::Die()
{
	bIsDead = true;
	AttackTarget = nullptr;
	bHasMoveCommand = false;

	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->DisableMovement();

	SetActorEnableCollision(false);

	Multicast_PlayDeathEffect();
	OnDied.Broadcast(this);
}

void AHRBHeroCharacter::Multicast_PlayHitEffect_Implementation()
{
	// TODO: spawn hit particle/sound
}

void AHRBHeroCharacter::Multicast_PlayDeathEffect_Implementation()
{
	SetActorHiddenInGame(true);
}

void AHRBHeroCharacter::OnRep_CurrentHP()
{
	OnHPChanged.Broadcast(CurrentHP, EffectiveStats.MaxHP);
}

void AHRBHeroCharacter::UpdateVisuals()
{
	if (HeroMesh)
	{
		UMaterialInstanceDynamic* DynMat = HeroMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMat)
		{
			DynMat->SetVectorParameterValue(TEXT("Base Color"), HeroColor);
		}
	}
}
