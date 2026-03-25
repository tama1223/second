// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HRBHealthBarComponent.generated.h"

class UHRBHealthBarWidget;

/**
 * UHRBHealthBarComponent
 *
 * 영웅 머리 위에 HP바를 표시하는 WidgetComponent.
 * 빌보드 방식, 월드 스페이스.
 */
UCLASS()
class SECOND_API UHRBHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UHRBHealthBarComponent();

	/** HP 업데이트 */
	void UpdateHP(float Current, float Max);

protected:
	virtual void BeginPlay() override;

private:
	/** HP바 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "HRB|UI")
	TSubclassOf<UHRBHealthBarWidget> HealthBarWidgetClass;
};
