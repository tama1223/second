// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/HRBGameModeBase.h"
#include "Game/HRBTestPawn.h"

AHRBGameModeBase::AHRBGameModeBase()
{
    DefaultPawnClass = AHRBTestPawn::StaticClass();
}

void AHRBGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("GameModeBase BeginPlay"));
}