// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "AI/HRBBTTask_FindTarget.h"

#include "Hero/HRBHeroCharacter.h"
#include "Hero/HRBEnemyHeroCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBBTTask_FindTarget)

UHRBBTTask_FindTarget::UHRBBTTask_FindTarget()
{
	NodeName = TEXT("Find Closest Target");

	// TargetActor 키 기본 이름 설정
	TargetActorKey.SelectedKeyName = FName(TEXT("TargetActor"));
}

EBTNodeResult::Type UHRBBTTask_FindTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC || !AIC->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	const FVector MyLocation = AIC->GetPawn()->GetActorLocation();
	const bool bIsEnemy = AIC->GetPawn()->IsA<AHRBEnemyHeroCharacter>();

	AHRBHeroCharacter* ClosestTarget = nullptr;
	float ClosestDistance = MAX_FLT;

	UWorld* World = AIC->GetWorld();
	if (!World)
	{
		return EBTNodeResult::Failed;
	}

	for (TActorIterator<AHRBHeroCharacter> It(World); It; ++It)
	{
		AHRBHeroCharacter* Candidate = *It;
		if (!Candidate || Candidate == AIC->GetPawn())
		{
			continue;
		}

		// 적 AI는 플레이어 측 영웅(AHRBEnemyHeroCharacter가 아닌)을 타겟으로
		// 플레이어 영웅이라면 적(AHRBEnemyHeroCharacter)을 타겟으로
		const bool bCandidateIsEnemy = Candidate->IsA<AHRBEnemyHeroCharacter>();
		if (bIsEnemy == bCandidateIsEnemy)
		{
			continue; // 같은 팀이면 스킵
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

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (ClosestTarget && BB)
	{
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, ClosestTarget);
		return EBTNodeResult::Succeeded;
	}

	// 타겟을 찾지 못함
	if (BB)
	{
		BB->ClearValue(TargetActorKey.SelectedKeyName);
	}
	return EBTNodeResult::Failed;
}

FString UHRBBTTask_FindTarget::GetStaticDescription() const
{
	return TEXT("Find the closest enemy HRBHeroCharacter and set as TargetActor");
}
