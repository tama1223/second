// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"

#include "HRBCameraComponent.generated.h"

class UHRBCameraMode;

DECLARE_DELEGATE_RetVal(TSubclassOf<UHRBCameraMode>, FHRBCameraModeDelegate);

/**
 * UHRBCameraComponent
 *
 * Lyra ULyraCameraComponent 패턴을 따르는 카메라 컴포넌트.
 * CameraMode 인스턴스를 관리하고, GetCameraView에서 뷰를 적용한다.
 */
UCLASS()
class SECOND_API UHRBCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UHRBCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 지정 액터에서 HRBCameraComponent를 찾는다. */
	UFUNCTION(BlueprintPure, Category = "HRB|Camera")
	static UHRBCameraComponent* FindCameraComponent(const AActor* Actor);

	/** 카메라가 추적하는 타겟 액터. 기본값은 Owner. */
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	/** 카메라 모드를 결정하는 델리게이트 */
	FHRBCameraModeDelegate DetermineCameraModeDelegate;

	/** 현재 활성 카메라 모드 인스턴스를 반환 (없으면 nullptr) */
	UHRBCameraMode* GetActiveCameraMode() const { return ActiveCameraMode; }

protected:
	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	/** 델리게이트로부터 카메라 모드를 결정하고 인스턴스를 갱신한다. */
	void UpdateCameraModes();

	/** CameraMode 클래스에서 인스턴스를 가져오거나 새로 생성한다. */
	UHRBCameraMode* GetCameraModeInstance(TSubclassOf<UHRBCameraMode> CameraModeClass);

protected:
	/** 현재 활성 카메라 모드 */
	UPROPERTY()
	TObjectPtr<UHRBCameraMode> ActiveCameraMode;

	/** 카메라 모드 인스턴스 캐시 */
	UPROPERTY()
	TArray<TObjectPtr<UHRBCameraMode>> CameraModeInstances;
};
