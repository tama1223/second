// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBHeroCharacter.h"

#include "Hero/HRBEnemyHeroCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/HRBHealthBarComponent.h"
#include "GameMode/HRBGameMode.h"
#include "TimerManager.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"
#include "UI/HRBDamageNumberActor.h"

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
}

void AHRBHeroCharacter::MoveToLocation(const FVector& Destination)
{
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

	// 피격 반응 (빨간 플래시)
	PlayHitReaction();

	// 데미지 숫자 팝업 스폰
	if (UWorld* World = GetWorld())
	{
		// 탑다운 시점에서 겹치지 않도록 랜덤 XY 오프셋 + Z 높이
		const float RandX = FMath::RandRange(-50.0f, 50.0f);
		const float RandY = FMath::RandRange(-50.0f, 50.0f);
		FVector SpawnLoc = GetActorLocation() + FVector(RandX, RandY, 200.0f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AHRBDamageNumberActor* DmgNum = World->SpawnActor<AHRBDamageNumberActor>(
			AHRBDamageNumberActor::StaticClass(), SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		if (DmgNum)
		{
			DmgNum->InitDamage(ActualDamage);
		}
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

	// 공격 이펙트
	PlayAttackEffect(Target);

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d attacked Hero %d for %.1f damage"),
		HeroIndex, Target->HeroIndex, AttackDamage);
}

// ==================== 공격이동 ====================

void AHRBHeroCharacter::AttackMoveToLocation(const FVector& Destination)
{
	if (bIsDead)
	{
		return;
	}

	// 기존 공격이동 정리
	StopAttackMove();

	bIsAttackMoving = true;
	AttackMoveDestination = Destination;
	AttackMoveTarget = nullptr;

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

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: AttackMove to (%.0f, %.0f, %.0f)"),
		HeroIndex, Destination.X, Destination.Y, Destination.Z);
}

void AHRBHeroCharacter::StopAttackMove()
{
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
		const float Distance = FVector::Dist(GetActorLocation(), AttackMoveTarget->GetActorLocation());

		if (Distance <= AttackRange)
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

	AHRBEnemyHeroCharacter* FoundEnemy = FindEnemyInDetectionRange();
	if (FoundEnemy)
	{
		// 적 발견 → 교전
		AttackMoveTarget = FoundEnemy;

		const float Distance = FVector::Dist(GetActorLocation(), FoundEnemy->GetActorLocation());
		if (Distance <= AttackRange)
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
		UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: AttackMove destination reached"), HeroIndex);
		StopAttackMove();
	}
	// 아직 이동 중이면 계속 스캔
}

AHRBEnemyHeroCharacter* AHRBHeroCharacter::FindEnemyInDetectionRange()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FVector MyLocation = GetActorLocation();
	AHRBEnemyHeroCharacter* ClosestEnemy = nullptr;
	float ClosestDistance = MAX_FLT;

	for (TActorIterator<AHRBEnemyHeroCharacter> It(World); It; ++It)
	{
		AHRBEnemyHeroCharacter* Enemy = *It;
		if (!Enemy || Enemy->bIsDead)
		{
			continue;
		}

		const float Distance = FVector::Dist(MyLocation, Enemy->GetActorLocation());
		if (Distance <= AttackMoveDetectionRange && Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestEnemy = Enemy;
		}
	}

	return ClosestEnemy;
}

// ==================== 전투 시각 피드백 ====================

void AHRBHeroCharacter::PlayHitReaction()
{
	if (!BodyMesh)
	{
		return;
	}

	// BodyMesh를 빨간색으로 변경
	UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
		TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	if (BaseMat)
	{
		UMaterialInstanceDynamic* HitMat = UMaterialInstanceDynamic::Create(BaseMat, this);
		if (HitMat)
		{
			HitMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
			BodyMesh->SetMaterial(0, HitMat);
		}
	}

	// 0.15초 후 원래 색으로 복귀
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitReactionTimer);
		World->GetTimerManager().SetTimer(
			HitReactionTimer, this, &ThisClass::ResetHitReaction, 0.15f, false);
	}
}

void AHRBHeroCharacter::ResetHitReaction()
{
	if (!BodyMesh)
	{
		return;
	}

	// 선택 상태에 따라 원래 머티리얼로 복귀
	if (bSelected && SelectedMaterial)
	{
		BodyMesh->SetMaterial(0, SelectedMaterial);
	}
	else if (NormalMaterial)
	{
		BodyMesh->SetMaterial(0, NormalMaterial);
	}
}

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
