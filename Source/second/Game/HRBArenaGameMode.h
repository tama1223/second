// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HRBArenaGameMode.generated.h"

/**
 * AHRBArenaGameMode
 *
 * 아레나 맵 전용 게임모드.
 * 기본 Pawn을 AHRBArenaCharacter로 설정한다.
 */
UCLASS()
class SECOND_API AHRBArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHRBArenaGameMode();

protected:
	virtual void BeginPlay() override;
};
