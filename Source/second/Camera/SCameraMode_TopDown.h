// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCameraMode_TopDown.generated.h"

/**
 * USCameraMode_TopDown
 *
 * Lyra의 ULyraCameraMode 패턴을 참고한 탑다운 카메라 모드.
 * 캐릭터 위에서 아래를 내려다보는 고정 탑다운 시점을 제공한다.
 * 마우스 휠로 줌 인/아웃이 가능하다.
 */
UCLASS(Blueprintable)
class SECOND_API USCameraMode_TopDown : public UObject
{
	GENERATED_BODY()

public:
	USCameraMode_TopDown();

	/** 카메라 뷰를 계산한다. */
	void UpdateView(const FVector& TargetLocation, float DeltaTime);

	/** 줌 입력을 처리한다. (양수 = 줌인, 음수 = 줌아웃) */
	void ApplyZoomInput(float ZoomDelta);

	/** 현재 카메라 위치 */
	FVector GetCameraLocation() const { return CachedLocation; }

	/** 현재 카메라 회전 */
	FRotator GetCameraRotation() const { return CachedRotation; }

	/** 현재 FOV */
	float GetFieldOfView() const { return FieldOfView; }

protected:
	// ---------- 뷰 설정 ----------

	/** 카메라가 바라보는 피치 각도 (도 단위, -90 = 완전 수직 탑다운) */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|View")
	float ViewPitch;

	/** 수평 FOV (도) */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|View")
	float FieldOfView;

	// ---------- 줌 설정 ----------

	/** 현재 줌 거리 (타겟으로부터의 높이) */
	UPROPERTY(VisibleAnywhere, Category = "TopDown|Zoom")
	float CurrentArmLength;

	/** 최소 줌 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float MinArmLength;

	/** 최대 줌 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float MaxArmLength;

	/** 줌 한 틱당 변화량 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float ZoomStep;

	/** 줌 보간 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown|Zoom")
	float ZoomInterpSpeed;

	// ---------- 내부 상태 ----------
	float TargetArmLength;
	FVector CachedLocation;
	FRotator CachedRotation;
};
