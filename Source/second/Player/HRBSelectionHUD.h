// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HRBSelectionHUD.generated.h"

/**
 * AHRBSelectionHUD
 *
 * 드래그 범위 선택 시 사각형을 화면에 그리는 HUD.
 */
UCLASS()
class SECOND_API AHRBSelectionHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
