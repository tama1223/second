// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SArenaGameMode.generated.h"

/**
 * ASArenaGameMode
 *
 * 아레나 맵 전용 게임모드.
 * 기본 Pawn을 ASArenaCharacter로 설정한다.
 */
UCLASS()
class SECOND_API ASArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASArenaGameMode();

protected:
	virtual void BeginPlay() override;
};
