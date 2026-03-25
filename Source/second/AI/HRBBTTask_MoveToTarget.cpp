// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "AI/HRBBTTask_MoveToTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBBTTask_MoveToTarget)

UHRBBTTask_MoveToTarget::UHRBBTTask_MoveToTarget()
{
	NodeName = TEXT("Move To Target");
	bNotifyTick = true;

	TargetActorKey.SelectedKeyName = FName(TEXT("TargetActor"));
}

EBTNodeResult::Type UHRBBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	EPathFollowingRequestResult::Type MoveResult = AIC->MoveToActor(
		TargetActor,
		AcceptableRadius,
		/*bStopOnOverlap=*/true,
		/*bUsePathfinding=*/true);

	if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}

	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}

	// 이동 중 - InProgress 상태로 Tick에서 완료 체크
	return EBTNodeResult::InProgress;
}

void UHRBBTTask_MoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UPathFollowingComponent* PathComp = AIC->GetPathFollowingComponent();
	if (!PathComp || PathComp->GetStatus() == EPathFollowingStatus::Idle)
	{
		// 이동이 완료되었거나 멈춤
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UHRBBTTask_MoveToTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (AIC)
	{
		AIC->StopMovement();
	}
	return EBTNodeResult::Aborted;
}

FString UHRBBTTask_MoveToTarget::GetStaticDescription() const
{
	return FString::Printf(TEXT("Move to TargetActor (Radius: %.0f)"), AcceptableRadius);
}
