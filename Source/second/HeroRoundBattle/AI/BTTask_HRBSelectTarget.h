#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HRBSelectTarget.generated.h"

/**
 * Selects the best enemy target using focus-fire strategy (lowest HP%).
 * Writes result to TargetActor blackboard key.
 */
UCLASS()
class SECOND_API UBTTask_HRBSelectTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HRBSelectTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};
