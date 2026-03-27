// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HRBDamageNumberActor.generated.h"

class UTextRenderComponent;

/**
 * AHRBDamageNumberActor
 *
 * 데미지 숫자를 월드에 표시하는 액터.
 * 빨간 숫자가 위로 떠오르며 1초 후 자동 소멸.
 */
UCLASS()
class SECOND_API AHRBDamageNumberActor : public AActor
{
	GENERATED_BODY()

public:
	AHRBDamageNumberActor();

	/** 데미지 값 설정 */
	void InitDamage(float Damage);

protected:
	virtual void BeginPlay() override;

private:
	/** 텍스트 렌더 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = "HRB|UI")
	TObjectPtr<UTextRenderComponent> TextRender;

	/** 위로 떠오르는 타이머 */
	FTimerHandle FloatTimer;

	/** 떠오르는 속도 (cm/tick) */
	float FloatSpeed = 3.0f;

	/** 떠오르기 타이머 콜백 */
	void FloatTick();
};
