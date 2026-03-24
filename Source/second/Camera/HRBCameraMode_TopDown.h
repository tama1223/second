// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/HRBCameraMode.h"

#include "HRBCameraMode_TopDown.generated.h"

/**
 * UHRBCameraMode_TopDown
 *
 * Lyra TopDownArena 카메라 패턴을 따르는 탑다운 카메라 모드.
 * 고정된 아레나 영역 위에서 내려다보는 시점을 제공한다.
 * 줌 인/아웃을 지원한다.
 */
UCLASS(Abstract, Blueprintable)
class SECOND_API UHRBCameraMode_TopDown : public UHRBCameraMode
{
	GENERATED_BODY()

public:
	UHRBCameraMode_TopDown();

	/** 줌 입력 처리 (양수 = 줌인, 음수 = 줌아웃) */
	void ApplyZoomInput(float ZoomDelta);

protected:
	//~ UHRBCameraMode interface
	virtual void UpdateView(float DeltaTime) override;
	//~ End of UHRBCameraMode interface

protected:
	/** 아레나 가로 크기 (원점 기준 반경) */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown")
	float ArenaWidth;

	/** 아레나 세로 크기 (원점 기준 반경) */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown")
	float ArenaHeight;

	/** 카메라 기본 회전 (탑다운 각도 결정) */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown")
	FRotator DefaultPivotRotation;

	/** 현재 줌 레벨 (카메라 높이 거리) */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float ZoomLevel;

	/** 최소 줌 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float MinZoom;

	/** 최대 줌 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float MaxZoom;

	/** 줌 한 틱당 변화량 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float ZoomStep;

	/** 줌 보간 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float ZoomInterpSpeed;

private:
	/** 줌 목표치 */
	float TargetZoomLevel;
};
