// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "UI/HRBHealthBarComponent.h"

#include "UI/HRBHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBHealthBarComponent)

UHRBHealthBarComponent::UHRBHealthBarComponent()
{
	// World 모드 + 빌보드 (TickComponent에서 카메라 향하도록 회전)
	SetWidgetSpace(EWidgetSpace::World);
	SetDrawAtDesiredSize(false);
	SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetDrawSize(FVector2D(100.0f, 10.0f));

	// 부모 회전 무시 — 빌보드를 위해 독립적으로 회전
	SetAbsolute(false, true, false);

	// Tick 활성화 (빌보드 회전용)
	PrimaryComponentTick.bCanEverTick = true;
}

void UHRBHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 카메라를 향하도록 회전 (빌보드)
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

			FVector Direction = CameraLocation - GetComponentLocation();
			SetWorldRotation(Direction.Rotation());
		}
	}
}

void UHRBHealthBarComponent::UpdateHP(float Current, float Max)
{
	UHRBHealthBarWidget* HPWidget = Cast<UHRBHealthBarWidget>(GetWidget());
	if (HPWidget)
	{
		HPWidget->UpdateHP(Current, Max);
	}
}
