#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_HRBUpdatePerception.generated.h"

/**
 * Updates blackboard with current combat state:
 * - SelfHPPercent: controlled hero's HP ratio
 * - TargetDistance: distance to current target
 * - Clears invalid targets
 */
UCLASS()
class SECOND_API UBTService_HRBUpdatePerception : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_HRBUpdatePerception();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SelfHPPercentKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetDistanceKey;
};
