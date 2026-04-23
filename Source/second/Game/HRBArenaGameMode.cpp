// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/HRBArenaGameMode.h"
#include "Game/HRBArenaCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBArenaGameMode)

AHRBArenaGameMode::AHRBArenaGameMode()
{
	DefaultPawnClass = AHRBArenaCharacter::StaticClass();
}

void AHRBArenaGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[HRBArenaGameMode] BeginPlay - Arena round game started"));
}
