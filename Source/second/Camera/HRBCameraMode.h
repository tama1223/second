// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "HRBCameraMode.generated.h"

class UHRBCameraComponent;

/**
 * FHRBCameraModeView
 *
 * Lyra FLyraCameraModeView 패턴.
 * 카메라 모드가 생산하는 뷰 데이터.
 */
USTRUCT()
struct FHRBCameraModeView
{
	GENERATED_BODY()

public:
	FHRBCameraModeView();

	void Blend(const FHRBCameraModeView& Other, float OtherWeight);

public:
	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/**
 * UHRBCameraMode
 *
 * Lyra ULyraCameraMode 패턴을 따르는 카메라 모드 기본 클래스.
 * UObject를 상속하며, HRBCameraComponent의 Outer로 생성된다.
 */
UCLASS(Abstract, NotBlueprintable)
class SECOND_API UHRBCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UHRBCameraMode();

	UHRBCameraComponent* GetHRBCameraComponent() const;

	virtual UWorld* GetWorld() const override;

	AActor* GetTargetActor() const;

	const FHRBCameraModeView& GetCameraModeView() const { return View; }

	/** 카메라 모드 활성화 시 호출 */
	virtual void OnActivation() {}

	/** 카메라 모드 비활성화 시 호출 */
	virtual void OnDeactivation() {}

	/** 뷰 계산 + 블렌딩 업데이트 */
	void UpdateCameraMode(float DeltaTime);

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

protected:
	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);

protected:
	/** 뷰 출력 데이터 */
	FHRBCameraModeView View;

	/** 수평 FOV (도) */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	/** 최소 뷰 피치 (도) */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	/** 최대 뷰 피치 (도) */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	/** 블렌드 인 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	/** 블렌드 알파 (선형 보간용 내부값) */
	float BlendAlpha;

	/** 블렌드 가중치 (최종 계산값) */
	float BlendWeight;
};
