// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBHeroCharacter.h"

#include "Hero/HRBEnemyHeroCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequence.h"
#include "Materials/MaterialInstance.h"
#include "UI/HRBHealthBarComponent.h"
#include "GameMode/HRBGameMode.h"
#include "TimerManager.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBHeroCharacter)

static TAutoConsoleVariable<int32> CVarHRBDebugCombat(
	TEXT("HRB.Debug.ShowCombat"),
	0,
	TEXT("Show combat debug visualization (0: off, 1: on)"),
	ECVF_Cheat);

AHRBHeroCharacter::AHRBHeroCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Replication 활성화 (PvP 데디 구조)
	bReplicates = true;
	SetReplicateMovement(true);

	// PlayerController로 Possess 하지 않음
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	// AIController는 자동 Possess (이동 명령에 필요)
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	// 캡슐 기본 크기
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// 선택 데칼 생성
	SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal"));
	SelectionDecal->SetupAttachment(GetRootComponent());
	SelectionDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f)); // 바닥을 향하도록
	SelectionDecal->DecalSize = FVector(64.0f, 64.0f, 64.0f);
	SelectionDecal->SetVisibility(false); // 기본 비선택 상태

	// 타겟 데칼 생성 (빨간 원 — 공격 대상 표시용)
	TargetDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("TargetDecal"));
	TargetDecal->SetupAttachment(GetRootComponent());
	TargetDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	TargetDecal->DecalSize = FVector(64.0f, 80.0f, 80.0f);
	TargetDecal->SetVisibility(false); // 기본 숨김

	// 이동 비활성화 (AI가 직접 제어할 때까지)
	GetCharacterMovement()->GravityScale = 1.0f;

	// HP 초기화
	CurrentHP = MaxHP;

	// HP바 컴포넌트
	HealthBarComp = CreateDefaultSubobject<UHRBHealthBarComponent>(TEXT("HealthBarComp"));
	HealthBarComp->SetupAttachment(GetRootComponent());

	// --- SkeletalMesh + Walking Animation ---
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkelMeshFinder(
		TEXT("/Game/HeroRoundBattle/Characaters/Orc/Meshes/Meshy_AI_Animation_Walking_withSkin.Meshy_AI_Animation_Walking_withSkin"));
	if (SkelMeshFinder.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkelMeshFinder.Object);
		// 메쉬 기본 자세 보정: ACharacter 규약(발바닥이 캡슐 바닥에 닿도록)
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}

	// 그림자 및 데칼 수신 off
	GetMesh()->SetCastShadow(false);
	GetMesh()->SetReceivesDecals(false);

	static ConstructorHelpers::FObjectFinder<UAnimSequence> WalkAnimFinder(
		TEXT("/Game/HeroRoundBattle/Characaters/Orc/Animations/Meshy_AI_Animation_Walking_withSkin_Anim.Meshy_AI_Animation_Walking_withSkin_Anim"));
	if (WalkAnimFinder.Succeeded())
	{
		WalkAnim = WalkAnimFinder.Object;
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		// 생성자 CDO에 확실히 저장되는 방식 — AnimToPlay 필드 직접 세팅
		GetMesh()->AnimationData.AnimToPlay = WalkAnim;
		GetMesh()->AnimationData.bSavedLooping = true;
		GetMesh()->AnimationData.bSavedPlaying = true;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> AttackAnimFinder(
		TEXT("/Game/HeroRoundBattle/Characaters/Orc/Animations/Meshy_AI_Animation_Attack_withSkin_Anim.Meshy_AI_Animation_Attack_withSkin_Anim"));
	if (AttackAnimFinder.Succeeded())
	{
		AttackAnim = AttackAnimFinder.Object;
	}

	// Hero 기본 Material (자식 Enemy가 Material_1로 override)
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> HeroMatFinder(
		TEXT("/Game/HeroRoundBattle/Characaters/Orc/Materials/Material_2.Material_2"));
	if (HeroMatFinder.Succeeded())
	{
		GetMesh()->SetMaterial(0, HeroMatFinder.Object);
	}
}

void AHRBHeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHRBHeroCharacter, HeroIndex);
	DOREPLIFETIME(AHRBHeroCharacter, CurrentHP);
	DOREPLIFETIME(AHRBHeroCharacter, bIsDead);  // RepNotify 변경 후에도 DOREPLIFETIME 그대로 사용
}

void AHRBHeroCharacter::OnRep_bIsDead()
{
	// 클라이언트 측 사망/부활 시각 처리
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(!bIsDead, true);  // bPropagateToChildren=true
	}
	SetActorEnableCollision(!bIsDead);
}

void AHRBHeroCharacter::Respawn(const FVector& NewLocation)
{
	// 서버 권한
	if (!HasAuthority())
	{
		return;
	}

	if (!bIsDead)
	{
		return;  // 살아있으면 무시
	}

	bIsDead = false;
	CurrentHP = MaxHP;

	// 위치/회전 reset
	SetActorLocation(NewLocation);
	SetActorRotation(FRotator::ZeroRotator);

	// 콜리전 on
	SetActorEnableCollision(true);

	// 메시 visible
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(true, true);  // bPropagateToChildren=true
	}

	// AIController 재 possess (SpawnDefaultController가 자동으로 없으면 스폰, 있으면 재사용)
	SpawnDefaultController();

	// HP바 갱신
	if (HealthBarComp)
	{
		HealthBarComp->UpdateHP(CurrentHP, MaxHP);
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d respawned at %s"),
		HeroIndex, *NewLocation.ToString());
}

void AHRBHeroCharacter::OnRep_CurrentHP()
{
	// 서버에서 CurrentHP가 변경되면 클라이언트에서 HP바 갱신
	if (HealthBarComp)
	{
		HealthBarComp->UpdateHP(CurrentHP, MaxHP);
	}
}

void AHRBHeroCharacter::MoveToLocation(const FVector& Destination)
{
	// 서버 Authoritative
	if (!HasAuthority())
	{
		return;
	}

	// 일반 이동 시 공격이동 취소
	StopAttackMove();

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

	// 일부 초기화 경로에서 생성자 애니 세팅이 누락될 수 있어 여기서 재보장
	if (WalkAnim && GetMesh())
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		GetMesh()->SetAnimation(WalkAnim);
		GetMesh()->Play(true);
	}

	// AIController가 없으면 스폰 (이동 명령에 필요)
	if (!GetController())
	{
		SpawnDefaultController();
	}

	// 선택 데칼은 숨김
	if (SelectionDecal)
	{
		SelectionDecal->SetVisibility(false);
	}

	// 타겟 데칼 머티리얼 설정 (빨간색)
	if (TargetDecal)
	{
		UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (BaseMat)
		{
			UMaterialInstanceDynamic* TargetDecalMat = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (TargetDecalMat)
			{
				TargetDecalMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
				TargetDecal->SetDecalMaterial(TargetDecalMat);
			}
		}
		TargetDecal->SetVisibility(false);
	}

	// HP 초기화 및 HP바 업데이트
	CurrentHP = MaxHP;
	if (HealthBarComp)
	{
		HealthBarComp->UpdateHP(CurrentHP, MaxHP);
	}
}

void AHRBHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if !UE_BUILD_SHIPPING
	UWorld* DebugWorld = GetWorld();

	// 선택 시각 피드백 (CVar 무관, 항상 표시) — 발 밑 노란 원
	if (bSelected && !bIsDead && DebugWorld)
	{
		const float HalfHeight = GetCapsuleComponent()
			? GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 90.f;
		const FVector FootLoc = GetActorLocation() - FVector(0.f, 0.f, HalfHeight - 5.f);
		DrawDebugCircle(DebugWorld, FootLoc, 80.f, 32,
			FColor::Yellow, false, 0.f, 0, 5.f,
			FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f), false);
	}

	if (CVarHRBDebugCombat.GetValueOnGameThread() <= 0 || bIsDead)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World) { return; }

	const FVector MyLoc = GetActorLocation();
	const FVector AxisX(1.f, 0.f, 0.f);
	const FVector AxisY(0.f, 1.f, 0.f);
	const float Life = 0.f; // 1프레임

	// 1) 공격 사거리 (초록)
	DrawDebugCircle(World, MyLoc, AttackRange, 48,
		FColor::Green, false, Life, 0, 1.5f, AxisX, AxisY, false);

	// 2) 공격이동 감지 범위 (노란)
	if (bIsAttackMoving)
	{
		DrawDebugCircle(World, MyLoc, AttackMoveDetectionRange, 64,
			FColor::Yellow, false, Life, 0, 1.5f, AxisX, AxisY, false);
	}

	// 3) 이동 목표 (파랑)
	if (!MoveDestination.IsNearlyZero())
	{
		DrawDebugLine(World, MyLoc, MoveDestination,
			FColor::Blue, false, Life, 0, 2.f);
		DrawDebugSphere(World, MoveDestination, 20.f, 12,
			FColor::Blue, false, Life, 0, 2.f);
	}

	// 4) 공격이동 목적지 (주황)
	if (bIsAttackMoving && !AttackMoveDestination.IsNearlyZero())
	{
		const FColor Orange(255, 140, 0);
		DrawDebugLine(World, MyLoc, AttackMoveDestination,
			Orange, false, Life, 0, 2.f);
		DrawDebugSphere(World, AttackMoveDestination, 20.f, 12,
			Orange, false, Life, 0, 2.f);
	}

	// 5) HP 오버레이
	const FString HPText = FString::Printf(TEXT("HP %d / %d"),
		FMath::RoundToInt(CurrentHP), FMath::RoundToInt(MaxHP));
	const FColor TextColor = IsA<AHRBEnemyHeroCharacter>() ? FColor::Red : FColor::White;
	DrawDebugString(World, MyLoc + FVector(0.f, 0.f, 150.f),
		HPText, nullptr, TextColor, 0.f, true, 1.2f);
#endif
}

void AHRBHeroCharacter::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;

	// 선택 데칼 on/off
	if (SelectionDecal)
	{
		SelectionDecal->SetVisibility(bSelected);
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: %s"),
		HeroIndex, bSelected ? TEXT("Selected") : TEXT("Deselected"));
}

float AHRBHeroCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	// 서버 Authoritative — 클라이언트에서 호출되어도 무시
	if (!HasAuthority())
	{
		return 0.0f;
	}

	if (bIsDead)
	{
		return 0.0f;
	}

	// 방어력 적용: 실제 데미지 = 공격력 - 방어력 (최소 1)
	const float ActualDamage = FMath::Max(DamageAmount - Defense, 1.0f);

	CurrentHP -= ActualDamage;

	const AActor* Attacker = DamageCauser ? DamageCauser : (EventInstigator ? EventInstigator->GetPawn() : nullptr);
	UE_LOG(LogTemp, Log, TEXT("[Combat] %s -> %s : %.0f damage (HP %.0f/%.0f)"),
		Attacker ? *GetNameSafe(Attacker) : TEXT("Unknown"),
		*GetNameSafe(this),
		ActualDamage, CurrentHP, MaxHP);

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
	// 서버 Authoritative
	if (!HasAuthority())
	{
		return;
	}

	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	// 공격이동 정지
	StopAttackMove();

	// 타겟 데칼 숨김
	SetTargeted(false);

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d has died!"), HeroIndex);

	// AI 정지
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
		AIC->UnPossess();
	}

	// 콜리전 비활성화
	SetActorEnableCollision(false);

	// 메시 숨김 (서버 측 — 클라이언트는 OnRep_bIsDead에서 처리)
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(false, true);  // bPropagateToChildren=true
	}

	// GameMode에 라운드 종료 확인 요청
	if (UWorld* World = GetWorld())
	{
		if (AHRBGameMode* GM = Cast<AHRBGameMode>(World->GetAuthGameMode()))
		{
			GM->CheckRoundEnd();
		}
	}
}

void AHRBHeroCharacter::Attack(AHRBHeroCharacter* Target)
{
	// 서버 Authoritative
	if (!HasAuthority())
	{
		return;
	}

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

	// 사거리 체크 (평면 2D 기준 — 디버그 원과 시각/논리 일치)
	if (!IsInAttackRange(Target))
	{
		const float Distance = FVector::Dist2D(GetActorLocation(), Target->GetActorLocation());
		UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: Target out of range (%.0f > %.0f)"),
			HeroIndex, Distance, AttackRange);
		return;
	}

	LastAttackTime = CurrentTime;

	// 적 방향으로 즉시 회전 (Yaw만, RTS 정석)
	const FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	if (!ToTarget.IsNearlyZero())
	{
		const float TargetYaw = ToTarget.Rotation().Yaw;
		SetActorRotation(FRotator(0.f, TargetYaw, 0.f));
	}

	// 데미지 적용
	FDamageEvent DamageEvent;
	Target->TakeDamage(AttackDamage, DamageEvent, GetController(), this);

#if !UE_BUILD_SHIPPING
	if (CVarHRBDebugCombat.GetValueOnGameThread() > 0 && Target)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(),
			FColor::Red, false, 0.3f, 0, 4.f);
	}
#endif

	// 공격 이펙트
	PlayAttackEffect(Target);

	// Attack 애니 1회 재생 → 전체 클라 동기화 (NetMulticast)
	Multicast_PlayAttackAnim();

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d attacked Hero %d for %.1f damage"),
		HeroIndex, Target->HeroIndex, AttackDamage);
}

void AHRBHeroCharacter::Multicast_PlayAttackAnim_Implementation()
{
	if (AttackAnim && GetMesh())
	{
		GetMesh()->SetAnimation(AttackAnim);
		GetMesh()->Play(false);
		const float Len = AttackAnim->GetPlayLength();
		GetWorld()->GetTimerManager().SetTimer(AttackAnimReturnTimer, this,
			&AHRBHeroCharacter::ReturnToWalkingAnim, Len, false);
	}
}

void AHRBHeroCharacter::ReturnToWalkingAnim()
{
	if (WalkAnim && GetMesh())
	{
		GetMesh()->SetAnimation(WalkAnim);
		GetMesh()->Play(true);
	}
}

// ==================== 공격이동 ====================

void AHRBHeroCharacter::AttackMoveToLocation(const FVector& Destination, AHRBHeroCharacter* InitialTarget)
{
	// 서버 Authoritative
	if (!HasAuthority())
	{
		return;
	}

	if (bIsDead)
	{
		return;
	}

	// 기존 공격이동 정리
	StopAttackMove();

	bIsAttackMoving = true;
	AttackMoveDestination = Destination;
	AttackMoveTarget = InitialTarget;

	// 목적지로 이동 시작
	MoveDestination = Destination;
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->MoveToLocation(Destination, 50.0f, /*bStopOnOverlap=*/true,
			/*bUsePathfinding=*/true, /*bProjectDestinationToNavigation=*/true);
	}

	// 적 탐색 타이머 시작
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			AttackMoveScanTimer, this, &ThisClass::AttackMoveScanTick,
			AttackMoveScanInterval, /*bLoop=*/true);
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: AttackMove to (%.0f, %.0f, %.0f), InitialTarget=%s"),
		HeroIndex, Destination.X, Destination.Y, Destination.Z,
		*GetNameSafe(InitialTarget));
}

void AHRBHeroCharacter::StopAttackMove()
{
	// 서버 Authoritative (타이머는 서버에서만 동작)
	if (!HasAuthority())
	{
		return;
	}

	bIsAttackMoving = false;
	AttackMoveTarget = nullptr;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackMoveScanTimer);
	}
}

void AHRBHeroCharacter::AttackMoveScanTick()
{
	if (bIsDead || !bIsAttackMoving)
	{
		StopAttackMove();
		return;
	}

	// 현재 교전 타겟이 있는 경우
	if (AttackMoveTarget && !AttackMoveTarget->bIsDead)
	{
		if (IsInAttackRange(AttackMoveTarget))
		{
			// 사거리 내 → 공격
			Attack(AttackMoveTarget);
		}
		else
		{
			// 사거리 밖 → 추적
			if (AAIController* AIC = Cast<AAIController>(GetController()))
			{
				AIC->MoveToActor(AttackMoveTarget, AttackRange * 0.8f);
			}
		}
		return;
	}

	// 타겟이 없거나 사망 → 새 적 탐색
	AttackMoveTarget = nullptr;

	AHRBHeroCharacter* FoundEnemy = FindEnemyInDetectionRange();
	if (FoundEnemy)
	{
		// 적 발견 → 교전
		AttackMoveTarget = FoundEnemy;

		if (IsInAttackRange(FoundEnemy))
		{
			Attack(FoundEnemy);
		}
		else
		{
			if (AAIController* AIC = Cast<AAIController>(GetController()))
			{
				AIC->MoveToActor(FoundEnemy, AttackRange * 0.8f);
			}
		}
		return;
	}

	// 적 미발견 → 목적지 도착 확인
	const float DistToDest = FVector::Dist2D(GetActorLocation(), AttackMoveDestination);
	if (DistToDest <= 100.0f)
	{
		// 목적지 도착 → 공격이동 완료
		StopAttackMove();
	}
	// 아직 이동 중이면 계속 스캔
}

AHRBHeroCharacter* AHRBHeroCharacter::FindEnemyInDetectionRange()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FVector MyLocation = GetActorLocation();
	AHRBHeroCharacter* ClosestEnemy = nullptr;
	float ClosestDistance = MAX_FLT;

	// 모든 HRBHeroCharacter를 순회하여 다른 팀 영웅만 적 후보로 취급
	for (TActorIterator<AHRBHeroCharacter> It(World); It; ++It)
	{
		AHRBHeroCharacter* Candidate = *It;
		if (!Candidate || Candidate == this || Candidate->bIsDead)
		{
			continue;
		}

		// 같은 팀이면 제외 (HeroIndex 10단위 그룹 기준)
		if (IsSameTeam(Candidate))
		{
			continue;
		}

		const float Distance = FVector::Dist(MyLocation, Candidate->GetActorLocation());
		if (Distance <= AttackMoveDetectionRange && Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestEnemy = Candidate;
		}
	}

	return ClosestEnemy;
}

// ==================== 전투 시각 피드백 ====================

void AHRBHeroCharacter::SetTargeted(bool bInTargeted)
{
	if (TargetDecal)
	{
		TargetDecal->SetVisibility(bInTargeted);
	}
}

void AHRBHeroCharacter::PlayAttackEffect(AActor* Target)
{
	if (!Target)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 피격자 위치에 작은 빨간 스피어를 0.2초간 스폰
	FVector EffectLocation = Target->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// 임시 액터 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* EffectActor = World->SpawnActor<AActor>(AActor::StaticClass(), EffectLocation, FRotator::ZeroRotator, SpawnParams);
	if (!EffectActor)
	{
		return;
	}

	// 스피어 메시 컴포넌트 추가
	UStaticMeshComponent* SphereMesh = NewObject<UStaticMeshComponent>(EffectActor, TEXT("HitSphere"));
	if (SphereMesh)
	{
		// RegisterComponent() 전에 루트 설정 필요
		EffectActor->SetRootComponent(SphereMesh);

		UStaticMesh* SphereSM = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere"));
		if (SphereSM)
		{
			SphereMesh->SetStaticMesh(SphereSM);
		}

		SphereMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereMesh->RegisterComponent();

		SphereMesh->SetWorldLocation(EffectLocation);
		SphereMesh->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));

		// 빨간색 머티리얼
		UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (BaseMat)
		{
			UMaterialInstanceDynamic* RedMat = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (RedMat)
			{
				RedMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.1f, 0.0f, 1.0f));
				SphereMesh->SetMaterial(0, RedMat);
			}
		}
	}

	// 0.2초 후 Destroy
	EffectActor->SetLifeSpan(0.2f);
}
