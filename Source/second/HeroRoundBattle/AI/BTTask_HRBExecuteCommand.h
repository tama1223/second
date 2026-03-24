#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HRBExecuteCommand.generated.h"

/**
 * Commands the controlled hero to attack the target stored in blackboard.
 */
UCLASS()
class SECOND_API UBTTask_HRBExecuteCommand : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HRBExecuteCommand();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};
