// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "UI/HRBHealthBarWidget.h"

#include "Components/ProgressBar.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBHealthBarWidget)

void UHRBHealthBarWidget::UpdateHP(float Current, float Max)
{
	if (HPBar)
	{
		const float Percent = (Max > 0.0f) ? FMath::Clamp(Current / Max, 0.0f, 1.0f) : 0.0f;
		HPBar->SetPercent(Percent);

		// HP 비율에 따라 색상 변경 (녹색 -> 노란색 -> 빨간색)
		FLinearColor BarColor;
		if (Percent > 0.5f)
		{
			BarColor = FLinearColor::LerpUsingHSV(FLinearColor::Yellow, FLinearColor::Green, (Percent - 0.5f) * 2.0f);
		}
		else
		{
			BarColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Yellow, Percent * 2.0f);
		}
		HPBar->SetFillColorAndOpacity(BarColor);
	}
}
