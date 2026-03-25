// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBHeroCharacter.h"

#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/HRBHealthBarComponent.h"
#include "GameMode/HRBGameMode.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBHeroCharacter)

AHRBHeroCharacter::AHRBHeroCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// PlayerController로 Possess 하지 않음
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	// AIController는 자동 Possess (이동 명령에 필요)
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	// 캡슐 기본 크기
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));

	// 시각적 표현용 실린더 메시
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetRootComponent());
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	BodyMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 1.5f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 콜리전은 캡슐이 담당

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(
		TEXT("/Engine/BasicShapes/Cylinder"));
	if (CylinderFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CylinderFinder.Object);
	}

	// 선택 데칼 생성
	SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal"));
	SelectionDecal->SetupAttachment(GetRootComponent());
	SelectionDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f)); // 바닥을 향하도록
	SelectionDecal->DecalSize = FVector(64.0f, 64.0f, 64.0f);
	SelectionDecal->SetVisibility(false); // 기본 비선택 상태

	// 이동 비활성화 (AI가 직접 제어할 때까지)
	GetCharacterMovement()->GravityScale = 1.0f;

	// HP 초기화
	CurrentHP = MaxHP;

	// HP바 컴포넌트
	HealthBarComp = CreateDefaultSubobject<UHRBHealthBarComponent>(TEXT("HealthBarComp"));
	HealthBarComp->SetupAttachment(GetRootComponent());
}

void AHRBHeroCharacter::MoveToLocation(const FVector& Destination)
{
	MoveDestination = Destination;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->MoveToLocation(Destination, 50.0f, /*bStopOnOverlap=*/true,
			/*bUsePathfinding=*/true, /*bProjectDestinationToNavigation=*/true);
		UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: MoveToLocation (%.0f, %.0f, %.0f)"),
			HeroIndex, Destination.X, Destination.Y, Destination.Z);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HRBHeroCharacter] Hero %d: No AIController, cannot move"), HeroIndex);
	}
}

void AHRBHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	// AIController가 없으면 스폰 (이동 명령에 필요)
	if (!GetController())
	{
		SpawnDefaultController();
	}

	// BasicShapeMaterial로 BodyMesh 기본 머티리얼 설정
	if (BodyMesh)
	{
		UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (BaseMat)
		{
			NormalMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			SelectedMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (NormalMaterial)
			{
				NormalMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)); // 회색
				BodyMesh->SetMaterial(0, NormalMaterial);
			}
			if (SelectedMaterial)
			{
				SelectedMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.0f, 1.0f, 0.2f, 1.0f)); // 초록
			}
		}
	}

	// 데칼은 사용하지 않음
	if (SelectionDecal)
	{
		SelectionDecal->SetVisibility(false);
	}

	// HP 초기화 및 HP바 업데이트
	CurrentHP = MaxHP;
	if (HealthBarComp)
	{
		HealthBarComp->UpdateHP(CurrentHP, MaxHP);
	}
}

void AHRBHeroCharacter::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;

	// BodyMesh 머티리얼로 선택 상태 표시 (회색 ↔ 초록)
	if (BodyMesh)
	{
		if (bSelected && SelectedMaterial)
		{
			BodyMesh->SetMaterial(0, SelectedMaterial);
		}
		else if (!bSelected && NormalMaterial)
		{
			BodyMesh->SetMaterial(0, NormalMaterial);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: %s"),
		HeroIndex, bSelected ? TEXT("Selected (green)") : TEXT("Deselected (grey)"));
}

float AHRBHeroCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	// 방어력 적용: 실제 데미지 = 공격력 - 방어력 (최소 1)
	const float ActualDamage = FMath::Max(DamageAmount - Defense, 1.0f);

	CurrentHP -= ActualDamage;

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d took %.1f damage (%.1f after defense). HP: %.1f/%.1f"),
		HeroIndex, DamageAmount, ActualDamage, CurrentHP, MaxHP);

	// HP바 업데이트
	if (HealthBarComp)
	{
		HealthBarComp->UpdateHP(CurrentHP, MaxHP);
	}

	if (CurrentHP <= 0.0f)
	{
		CurrentHP = 0.0f;
		Die();
	}

	return ActualDamage;
}

void AHRBHeroCharacter::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d has died!"), HeroIndex);

	// AI 정지
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
		AIC->UnPossess();
	}

	// 콜리전 비활성화
	SetActorEnableCollision(false);

	// GameMode에 라운드 종료 확인 요청
	if (UWorld* World = GetWorld())
	{
		if (AHRBGameMode* GM = Cast<AHRBGameMode>(World->GetAuthGameMode()))
		{
			GM->CheckRoundEnd();
		}
	}

	// 1초 후 Destroy
	FTimerHandle DestroyTimerHandle;
	TWeakObjectPtr<AHRBHeroCharacter> WeakThis(this);
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, [WeakThis]()
	{
		if (WeakThis.IsValid())
		{
			WeakThis->Destroy();
		}
	}, 1.0f, false);
}

void AHRBHeroCharacter::Attack(AHRBHeroCharacter* Target)
{
	if (!Target || Target->bIsDead || bIsDead)
	{
		return;
	}

	// 쿨다운 체크
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return;
	}

	// 사거리 체크
	const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance > AttackRange)
	{
		UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: Target out of range (%.0f > %.0f)"),
			HeroIndex, Distance, AttackRange);
		return;
	}

	LastAttackTime = CurrentTime;

	// 데미지 적용
	FDamageEvent DamageEvent;
	Target->TakeDamage(AttackDamage, DamageEvent, GetController(), this);

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d attacked Hero %d for %.1f damage"),
		HeroIndex, Target->HeroIndex, AttackDamage);
}
