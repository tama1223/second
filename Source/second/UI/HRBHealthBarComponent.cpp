// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "UI/HRBHealthBarComponent.h"

#include "UI/HRBHealthBarWidget.h"
#include "Components/ProgressBar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBHealthBarComponent)

UHRBHealthBarComponent::UHRBHealthBarComponent()
{
	// 빌보드, 월드 스페이스
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 기본 크기
	SetDrawSize(FVector2D(100.0f, 20.0f));
}

void UHRBHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	// 위젯 클래스가 설정되지 않았으면 동적 생성된 프로그레스바 사용
	// (WidgetBlueprint가 없을 때의 폴백)
	if (!HealthBarWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HRBHealthBarComponent] HealthBarWidgetClass not set. HP bar will not display."));
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
