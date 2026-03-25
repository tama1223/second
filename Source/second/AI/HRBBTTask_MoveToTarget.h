// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "HRBBTTask_MoveToTarget.generated.h"

/**
 * UHRBBTTask_MoveToTarget
 *
 * Blackboard의 TargetActor 위치로 이동.
 */
UCLASS()
class SECOND_API UHRBBTTask_MoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UHRBBTTask_MoveToTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** Blackboard Key: TargetActor */
	UPROPERTY(EditAnywhere, Category = "HRB|AI")
	FBlackboardKeySelector TargetActorKey;

	/** 허용 거리 (이 거리 이내면 도착으로 판정) */
	UPROPERTY(EditAnywhere, Category = "HRB|AI")
	float AcceptableRadius = 100.0f;
};
