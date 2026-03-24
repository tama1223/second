// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Camera/HRBCameraMode.h"

#include "Camera/HRBCameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBCameraMode)

namespace HRBCameraDefaults
{
	static constexpr float DefaultFOV = 80.0f;
	static constexpr float DefaultPitchMin = -89.9f;
	static constexpr float DefaultPitchMax = 89.9f;
}

//////////////////////////////////////////////////////////////////////////
// FHRBCameraModeView
//////////////////////////////////////////////////////////////////////////

FHRBCameraModeView::FHRBCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(HRBCameraDefaults::DefaultFOV)
{
}

void FHRBCameraModeView::Blend(const FHRBCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	else if (OtherWeight >= 1.0f)
	{
		*this = Other;
		return;
	}

	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

//////////////////////////////////////////////////////////////////////////
// UHRBCameraMode
//////////////////////////////////////////////////////////////////////////

UHRBCameraMode::UHRBCameraMode()
{
	FieldOfView = HRBCameraDefaults::DefaultFOV;
	ViewPitchMin = HRBCameraDefaults::DefaultPitchMin;
	ViewPitchMax = HRBCameraDefaults::DefaultPitchMax;

	BlendTime = 0.5f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
}

UHRBCameraComponent* UHRBCameraMode::GetHRBCameraComponent() const
{
	return CastChecked<UHRBCameraComponent>(GetOuter());
}

UWorld* UHRBCameraMode::GetWorld() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : GetOuter()->GetWorld();
}

AActor* UHRBCameraMode::GetTargetActor() const
{
	const UHRBCameraComponent* CameraComponent = GetHRBCameraComponent();
	return CameraComponent->GetTargetActor();
}

FVector UHRBCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// 캐릭터의 경우 크라우칭 등에 따른 높이 보정
		if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetPawn))
		{
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
			check(TargetCharacterCDO);

			const UCapsuleComponent* CapsuleComp = TargetCharacter->GetCapsuleComponent();
			check(CapsuleComp);

			const UCapsuleComponent* CapsuleCompCDO = TargetCharacterCDO->GetCapsuleComponent();
			check(CapsuleCompCDO);

			const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();
			const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
			const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

			return TargetCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
		}

		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UHRBCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UHRBCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateBlending(DeltaTime);
}

void UHRBCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
}

void UHRBCameraMode::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);

	// BlendWeight에서 BlendAlpha를 역산 (EaseOut 기준)
	// Step 1에서는 간소화: 단순 선형 역산
	BlendAlpha = BlendWeight;
}

void UHRBCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.0f)
	{
		BlendAlpha += (DeltaTime / BlendTime);
		BlendAlpha = FMath::Min(BlendAlpha, 1.0f);
	}
	else
	{
		BlendAlpha = 1.0f;
	}

	// Step 1에서는 선형 블렌딩만 사용
	BlendWeight = BlendAlpha;
}
