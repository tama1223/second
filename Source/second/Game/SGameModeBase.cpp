// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGameModeBase.h"
#include "Game/STestPawn.h"

ASGameModeBase::ASGameModeBase()
{
    DefaultPawnClass = ASTestPawn::StaticClass();
}

void ASGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("GameModeBase BeginPlay"));
}