// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Camera/HRBCameraComponent.h"

#include "Camera/HRBCameraMode.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBCameraComponent)

UHRBCameraComponent::UHRBCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActiveCameraMode = nullptr;
}

UHRBCameraComponent* UHRBCameraComponent::FindCameraComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UHRBCameraComponent>() : nullptr;
}

void UHRBCameraComponent::OnRegister()
{
	Super::OnRegister();
}

void UHRBCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	// 카메라 모드 결정
	UpdateCameraModes();

	if (ActiveCameraMode)
	{
		// 카메라 모드 업데이트 (뷰 계산 + 블렌딩)
		ActiveCameraMode->UpdateCameraMode(DeltaTime);

		const FHRBCameraModeView& CameraModeView = ActiveCameraMode->GetCameraModeView();

		// PlayerController의 ControlRotation 동기화
		if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
		{
			if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
			{
				PC->SetControlRotation(CameraModeView.ControlRotation);
			}
		}

		// 카메라 컴포넌트에 뷰 적용
		SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);
		FieldOfView = CameraModeView.FieldOfView;

		// DesiredView 채움
		DesiredView.Location = CameraModeView.Location;
		DesiredView.Rotation = CameraModeView.Rotation;
		DesiredView.FOV = CameraModeView.FieldOfView;
		DesiredView.OrthoWidth = OrthoWidth;
		DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
		DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
		DesiredView.AspectRatio = AspectRatio;
		DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
		DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
		DesiredView.ProjectionMode = ProjectionMode;

		DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
		if (PostProcessBlendWeight > 0.0f)
		{
			DesiredView.PostProcessSettings = PostProcessSettings;
		}
	}
	else
	{
		// 카메라 모드가 없으면 기본 동작
		Super::GetCameraView(DeltaTime, DesiredView);
	}
}

void UHRBCameraComponent::UpdateCameraModes()
{
	if (DetermineCameraModeDelegate.IsBound())
	{
		if (const TSubclassOf<UHRBCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			UHRBCameraMode* Instance = GetCameraModeInstance(CameraMode);
			if (Instance != ActiveCameraMode)
			{
				if (ActiveCameraMode)
				{
					ActiveCameraMode->OnDeactivation();
				}

				ActiveCameraMode = Instance;
				ActiveCameraMode->OnActivation();
			}
		}
	}
}

UHRBCameraMode* UHRBCameraComponent::GetCameraModeInstance(TSubclassOf<UHRBCameraMode> CameraModeClass)
{
	check(CameraModeClass);

	// 캐시에서 기존 인스턴스 검색
	for (UHRBCameraMode* CameraMode : CameraModeInstances)
	{
		if (CameraMode != nullptr && CameraMode->GetClass() == CameraModeClass)
		{
			return CameraMode;
		}
	}

	// 새 인스턴스 생성 (Outer를 이 컴포넌트로 설정 - Lyra 패턴)
	UHRBCameraMode* NewCameraMode = NewObject<UHRBCameraMode>(this, CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}
