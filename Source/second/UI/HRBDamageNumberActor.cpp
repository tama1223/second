// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "UI/HRBDamageNumberActor.h"

#include "Components/TextRenderComponent.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBDamageNumberActor)

AHRBDamageNumberActor::AHRBDamageNumberActor()
{
	PrimaryActorTick.bCanEverTick = false;

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	SetRootComponent(TextRender);

	TextRender->SetHorizontalAlignment(EHTA_Center);
	TextRender->SetVerticalAlignment(EVRTA_TextCenter);
	TextRender->SetWorldSize(80.0f);  // 탑다운에서 잘 보이도록 크게
	TextRender->SetTextRenderColor(FColor(255, 50, 50, 255)); // 밝은 빨강
}

void AHRBDamageNumberActor::BeginPlay()
{
	Super::BeginPlay();

	// 탑다운 카메라를 향하도록 회전 (위에서 내려다보는 시점에서 읽히게)
	SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f));

	// 0.05초마다 위로 떠오르기
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			FloatTimer, this, &ThisClass::FloatTick, 0.05f, /*bLoop=*/true);
	}

	// 1.5초 후 자동 소멸
	SetLifeSpan(1.5f);
}

void AHRBDamageNumberActor::InitDamage(float Damage)
{
	if (TextRender)
	{
		// "-15" 형태로 표시
		FString DamageText = FString::Printf(TEXT("-%.0f"), Damage);
		TextRender->SetText(FText::FromString(DamageText));
	}
}

void AHRBDamageNumberActor::FloatTick()
{
	// 위로 떠오르기
	FVector CurrentLoc = GetActorLocation();
	CurrentLoc.Z += FloatSpeed;
	SetActorLocation(CurrentLoc);
}
