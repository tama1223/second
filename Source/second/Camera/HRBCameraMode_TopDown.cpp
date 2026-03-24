// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Camera/HRBCameraMode_TopDown.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBCameraMode_TopDown)

UHRBCameraMode_TopDown::UHRBCameraMode_TopDown()
{
	// 아레나 크기
	ArenaWidth = 1000.0f;
	ArenaHeight = 1000.0f;

	// 탑다운 시점: 피치 -70도, Yaw 0
	DefaultPivotRotation = FRotator(-70.0f, 0.0f, 0.0f);

	// 줌 설정
	ZoomLevel = 2000.0f;
	TargetZoomLevel = 2000.0f;
	MinZoom = 800.0f;
	MaxZoom = 5000.0f;
	ZoomStep = 200.0f;
	ZoomInterpSpeed = 8.0f;

	// FOV override
	FieldOfView = 60.0f;
}

void UHRBCameraMode_TopDown::ApplyZoomInput(float ZoomDelta)
{
	// 양수 = 줌인 (거리 감소), 음수 = 줌아웃 (거리 증가)
	TargetZoomLevel = FMath::Clamp(TargetZoomLevel - ZoomDelta * ZoomStep, MinZoom, MaxZoom);
}

void UHRBCameraMode_TopDown::UpdateView(float DeltaTime)
{
	// 줌 보간
	ZoomLevel = FMath::FInterpTo(ZoomLevel, TargetZoomLevel, DeltaTime, ZoomInterpSpeed);

	// Lyra TopDownArena 패턴: 아레나 중심에서 DefaultPivotRotation의 반대 방향으로 ZoomLevel만큼 카메라를 배치
	// 타겟 액터(캐릭터)를 추적하는 방식
	const AActor* TargetActor = GetTargetActor();
	FVector PivotLocation = TargetActor ? TargetActor->GetActorLocation() : FVector::ZeroVector;

	// DefaultPivotRotation 방향의 반대로 카메라를 배치
	FVector CameraLocation = PivotLocation - DefaultPivotRotation.Vector() * ZoomLevel;

	View.Location = CameraLocation;
	View.Rotation = DefaultPivotRotation;
	View.ControlRotation = DefaultPivotRotation;
	View.FieldOfView = FieldOfView;
}
