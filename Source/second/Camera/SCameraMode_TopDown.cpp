// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/SCameraMode_TopDown.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SCameraMode_TopDown)

USCameraMode_TopDown::USCameraMode_TopDown()
{
	// 탑다운 기본 설정
	ViewPitch = -70.0f;  // 거의 수직에 가까운 탑다운 (완전 수직 = -90)
	FieldOfView = 60.0f;

	// 줌 설정
	CurrentArmLength = 2000.0f;
	TargetArmLength = 2000.0f;
	MinArmLength = 800.0f;
	MaxArmLength = 5000.0f;
	ZoomStep = 200.0f;
	ZoomInterpSpeed = 8.0f;

	// 캐시 초기화
	CachedLocation = FVector::ZeroVector;
	CachedRotation = FRotator(ViewPitch, 0.0f, 0.0f);
}

void USCameraMode_TopDown::UpdateView(const FVector& TargetLocation, float DeltaTime)
{
	// 줌 보간
	CurrentArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, ZoomInterpSpeed);

	// 탑다운 카메라 위치 계산
	// ViewPitch 각도를 기반으로 카메라 오프셋 계산
	const float PitchRad = FMath::DegreesToRadians(ViewPitch);

	// Pitch가 -70도이면 약간 뒤로 오프셋이 생김 (완전 수직이 아니므로)
	const float HorizontalDist = CurrentArmLength * FMath::Cos(PitchRad);
	const float VerticalDist = -CurrentArmLength * FMath::Sin(PitchRad); // Sin(-70) < 0 이므로 부호 반전

	// 카메라는 타겟의 뒤쪽 위에 위치 (Yaw = 0 기준으로 -X 방향)
	CachedLocation = TargetLocation + FVector(-HorizontalDist, 0.0f, VerticalDist);
	CachedRotation = FRotator(ViewPitch, 0.0f, 0.0f);
}

void USCameraMode_TopDown::ApplyZoomInput(float ZoomDelta)
{
	// 양수 = 줌인 (거리 감소), 음수 = 줌아웃 (거리 증가)
	TargetArmLength = FMath::Clamp(TargetArmLength - ZoomDelta * ZoomStep, MinArmLength, MaxArmLength);
}
