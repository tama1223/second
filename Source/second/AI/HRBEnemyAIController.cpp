// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "AI/HRBEnemyAIController.h"

#include "Hero/HRBHeroCharacter.h"
#include "Hero/HRBEnemyHeroCharacter.h"
#include "EngineUtils.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBEnemyAIController)

AHRBEnemyAIController::AHRBEnemyAIController()
{
}

void AHRBEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 타이머 기반 AI 로직 시작
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			AITickTimer, this, &ThisClass::AITick, AITickInterval, /*bLoop=*/true);

		UE_LOG(LogTemp, Log, TEXT("[HRBEnemyAIController] AI timer started for %s (interval: %.2fs)"),
			*GetNameSafe(InPawn), AITickInterval);
	}
}

void AHRBEnemyAIController::OnUnPossess()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AITickTimer);
	}

	Super::OnUnPossess();
}

void AHRBEnemyAIController::AITick()
{
	AHRBHeroCharacter* MyHero = Cast<AHRBHeroCharacter>(GetPawn());
	if (!MyHero || MyHero->bIsDead)
	{
		return;
	}

	// 1. 타겟 찾기 (가장 가까운 적)
	CurrentTarget = FindClosestTarget();
	if (!CurrentTarget)
	{
		return;
	}

	const float Distance = FVector::Dist(MyHero->GetActorLocation(), CurrentTarget->GetActorLocation());

	// 2. 사거리 내면 공격
	if (Distance <= MyHero->AttackRange)
	{
		MyHero->Attack(CurrentTarget);
	}
	else
	{
		// 3. 사거리 밖이면 이동
		MoveToActor(CurrentTarget, MyHero->AttackRange * 0.8f);
	}
}

AHRBHeroCharacter* AHRBEnemyAIController::FindClosestTarget()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return nullptr;
	}

	const FVector MyLocation = MyPawn->GetActorLocation();
	const bool bIsEnemy = MyPawn->IsA<AHRBEnemyHeroCharacter>();

	AHRBHeroCharacter* ClosestTarget = nullptr;
	float ClosestDistance = MAX_FLT;

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<AHRBHeroCharacter> It(World); It; ++It)
	{
		AHRBHeroCharacter* Candidate = *It;
		if (!Candidate || Candidate == MyPawn)
		{
			continue;
		}

		// 같은 팀이면 스킵
		const bool bCandidateIsEnemy = Candidate->IsA<AHRBEnemyHeroCharacter>();
		if (bIsEnemy == bCandidateIsEnemy)
		{
			continue;
		}

		// 사망한 대상 스킵
		if (Candidate->bIsDead)
		{
			continue;
		}

		const float Distance = FVector::Dist(MyLocation, Candidate->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestTarget = Candidate;
		}
	}

	return ClosestTarget;
}
