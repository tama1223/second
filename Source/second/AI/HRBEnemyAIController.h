// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HRBEnemyAIController.generated.h"

class AHRBHeroCharacter;

/**
 * AHRBEnemyAIController
 *
 * 적 영웅을 제어하는 AI Controller.
 * 타이머 기반으로 주기적으로 타겟을 찾고, 사거리 내면 공격, 밖이면 이동한다.
 */
UCLASS()
class SECOND_API AHRBEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AHRBEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	/** AI 판단 주기 타이머 */
	FTimerHandle AITickTimer;

	/** AI 판단 주기 (초) */
	float AITickInterval = 0.5f;

	/** 현재 타겟 */
	UPROPERTY()
	TObjectPtr<AHRBHeroCharacter> CurrentTarget;

	/** 타이머 콜백: 타겟 탐색 → 공격 or 이동 */
	void AITick();

	/** 가장 가까운 적 팀 영웅을 찾아 반환 */
	AHRBHeroCharacter* FindClosestTarget();
};
