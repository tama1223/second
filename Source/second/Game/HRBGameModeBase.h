// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HRBGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class SECOND_API AHRBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHRBGameModeBase();

protected:
	virtual void BeginPlay() override;

};
