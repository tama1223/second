// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "HRBBTTask_FindTarget.generated.h"

/**
 * UHRBBTTask_FindTarget
 *
 * 가장 가까운 적(플레이어 측) HRBHeroCharacter를 찾아서 Blackboard에 설정.
 */
UCLASS()
class SECOND_API UHRBBTTask_FindTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UHRBBTTask_FindTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	/** Blackboard Key: TargetActor */
	UPROPERTY(EditAnywhere, Category = "HRB|AI")
	FBlackboardKeySelector TargetActorKey;
};
