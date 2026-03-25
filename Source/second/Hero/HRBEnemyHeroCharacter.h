// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hero/HRBHeroCharacter.h"
#include "HRBEnemyHeroCharacter.generated.h"

class AHRBEnemyAIController;

/**
 * AHRBEnemyHeroCharacter
 *
 * 적 AI가 제어하는 영웅 캐릭터.
 * 빨간색 실린더로 시각적 구분. HRBEnemyAIController가 행동트리로 제어.
 */
UCLASS()
class SECOND_API AHRBEnemyHeroCharacter : public AHRBHeroCharacter
{
	GENERATED_BODY()

public:
	AHRBEnemyHeroCharacter();

protected:
	virtual void BeginPlay() override;
};
