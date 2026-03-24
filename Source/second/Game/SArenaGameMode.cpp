// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/SArenaGameMode.h"
#include "Game/SArenaCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SArenaGameMode)

ASArenaGameMode::ASArenaGameMode()
{
	DefaultPawnClass = ASArenaCharacter::StaticClass();
}

void ASArenaGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[SArenaGameMode] BeginPlay - Arena round game started"));
}
