// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Player/HRBSelectionHUD.h"

#include "Player/HRBPlayerController.h"
#include "Engine/Canvas.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBSelectionHUD)

void AHRBSelectionHUD::DrawHUD()
{
	Super::DrawHUD();

	AHRBPlayerController* PC = Cast<AHRBPlayerController>(GetOwningPlayerController());
	if (!PC)
	{
		return;
	}

	// 드래그 중인 경우에만 사각형 그리기
	// bIsDragging가 false여도 마우스 버튼이 눌린 상태에서 threshold를 넘었는지 확인
	if (!PC->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		return;
	}

	const float DragDistance = FVector2D::Distance(PC->DragStartScreen, PC->DragCurrentScreen);
	if (DragDistance < 10.0f)
	{
		return;
	}

	// 드래그 중으로 전환 (PC가 PlayerTick에서 갱신)
	PC->bIsDragging = true;

	const FVector2D Start = PC->DragStartScreen;
	const FVector2D Current = PC->DragCurrentScreen;

	const float MinX = FMath::Min(Start.X, Current.X);
	const float MaxX = FMath::Max(Start.X, Current.X);
	const float MinY = FMath::Min(Start.Y, Current.Y);
	const float MaxY = FMath::Max(Start.Y, Current.Y);

	const float Width = MaxX - MinX;
	const float Height = MaxY - MinY;

	// 반투명 채우기
	DrawRect(FLinearColor(0.0f, 1.0f, 0.0f, 0.15f), MinX, MinY, Width, Height);

	// 테두리 그리기
	const FLinearColor BorderColor(0.0f, 1.0f, 0.0f, 0.8f);
	DrawLine(MinX, MinY, MaxX, MinY, BorderColor, 2.0f); // 상단
	DrawLine(MaxX, MinY, MaxX, MaxY, BorderColor, 2.0f); // 우측
	DrawLine(MaxX, MaxY, MinX, MaxY, BorderColor, 2.0f); // 하단
	DrawLine(MinX, MaxY, MinX, MinY, BorderColor, 2.0f); // 좌측
}
