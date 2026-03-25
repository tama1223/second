// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HRBEnemyAIController.generated.h"

class UBehaviorTree;

/**
 * AHRBEnemyAIController
 *
 * 적 영웅을 제어하는 AI Controller.
 * BeginPlay에서 Behavior Tree를 실행한다.
 */
UCLASS()
class SECOND_API AHRBEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AHRBEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	/** 실행할 행동 트리 (에디터에서 설정) */
	UPROPERTY(EditDefaultsOnly, Category = "HRB|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

private:
	/** BT 설정 초기화 */
	void SetupBehaviorTree();
};
