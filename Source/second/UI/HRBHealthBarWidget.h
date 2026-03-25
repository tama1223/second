// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HRBHealthBarWidget.generated.h"

class UProgressBar;

/**
 * UHRBHealthBarWidget
 *
 * 영웅의 HP바를 표시하는 위젯.
 */
UCLASS()
class SECOND_API UHRBHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** HP 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "HRB|UI")
	void UpdateHP(float Current, float Max);

protected:
	/** HP 프로그레스바 - WidgetBlueprint에서 바인딩 */
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HRB|UI")
	TObjectPtr<UProgressBar> HPBar;
};
