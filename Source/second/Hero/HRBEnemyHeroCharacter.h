// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hero/HRBHeroCharacter.h"
#include "HRBEnemyHeroCharacter.generated.h"

/**
 * AHRBEnemyHeroCharacter
 *
 * 적 팀(Team 1) 영웅 캐릭터. Material_1(검정 오크)로 시각 구분.
 * AI 없음 — 사람 PlayerController가 조종.
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
