// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "UI/HRBHealthBarComponent.h"

#include "UI/HRBHealthBarWidget.h"
#include "Components/ProgressBar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBHealthBarComponent)

UHRBHealthBarComponent::UHRBHealthBarComponent()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(false);
	SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetDrawSize(FVector2D(100.0f, 10.0f));
}

void UHRBHealthBarComponent::UpdateHP(float Current, float Max)
{
	UHRBHealthBarWidget* HPWidget = Cast<UHRBHealthBarWidget>(GetWidget());
	if (HPWidget)
	{
		HPWidget->UpdateHP(Current, Max);
	}
}
