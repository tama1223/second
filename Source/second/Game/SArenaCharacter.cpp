// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/SArenaCharacter.h"

#include "Camera/HRBCameraComponent.h"
#include "Camera/HRBCameraMode.h"
#include "Camera/HRBCameraMode_TopDown.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SArenaCharacter)

ASArenaCharacter::ASArenaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 캡슐 기본 크기
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// 컨트롤러 회전을 캐릭터에 적용하지 않음 (탑다운이므로)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// CharacterMovement 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	// HRBCameraComponent 생성 (Lyra 패턴)
	HRBCameraComp = CreateDefaultSubobject<UHRBCameraComponent>(TEXT("HRBCamera"));
	HRBCameraComp->SetupAttachment(GetRootComponent());
	HRBCameraComp->SetUsingAbsoluteLocation(true);
	HRBCameraComp->SetUsingAbsoluteRotation(true);
	HRBCameraComp->bUsePawnControlRotation = false;

	// 기본 카메라 모드 (BP에서 override 가능)
	DefaultCameraModeClass = nullptr;

	// Auto-possess
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 카메라 모드 결정 델리게이트 바인딩
	if (HRBCameraComp)
	{
		HRBCameraComp->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
	}
}

TSubclassOf<UHRBCameraMode> ASArenaCharacter::DetermineCameraMode() const
{
	return DefaultCameraModeClass;
}

void ASArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ---- 런타임에서 InputAction 생성 ----

	// 이동 (WASD) - Axis2D
	IA_Move = NewObject<UInputAction>(this, TEXT("IA_Move"));
	IA_Move->ValueType = EInputActionValueType::Axis2D;

	// 줌 (마우스 휠) - Axis1D
	IA_Zoom = NewObject<UInputAction>(this, TEXT("IA_Zoom"));
	IA_Zoom->ValueType = EInputActionValueType::Axis1D;

	// ---- MappingContext 생성 ----
	IMC_Arena = NewObject<UInputMappingContext>(this, TEXT("IMC_Arena"));

	// --- Move (WASD) ---
	{
		FEnhancedActionKeyMapping& W = IMC_Arena->MapKey(IA_Move, EKeys::W);
		auto* SwizzleMod = NewObject<UInputModifierSwizzleAxis>(this);
		SwizzleMod->Order = EInputAxisSwizzle::YXZ;
		W.Modifiers.Add(SwizzleMod);
	}
	{
		FEnhancedActionKeyMapping& S = IMC_Arena->MapKey(IA_Move, EKeys::S);
		auto* SwizzleMod = NewObject<UInputModifierSwizzleAxis>(this);
		SwizzleMod->Order = EInputAxisSwizzle::YXZ;
		S.Modifiers.Add(SwizzleMod);
		auto* Negate = NewObject<UInputModifierNegate>(this);
		S.Modifiers.Add(Negate);
	}
	{
		IMC_Arena->MapKey(IA_Move, EKeys::D);
	}
	{
		FEnhancedActionKeyMapping& A = IMC_Arena->MapKey(IA_Move, EKeys::A);
		auto* Negate = NewObject<UInputModifierNegate>(this);
		A.Modifiers.Add(Negate);
	}

	// --- Zoom (마우스 휠) ---
	{
		IMC_Arena->MapKey(IA_Zoom, EKeys::MouseWheelAxis);
	}

	// ---- 매핑 컨텍스트 등록 ----
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Arena, 0);
		}
	}

	// ---- 액션 바인딩 ----
	if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASArenaCharacter::HandleMove);
		EIC->BindAction(IA_Zoom, ETriggerEvent::Triggered, this, &ASArenaCharacter::HandleZoom);
	}
}

void ASArenaCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	if (Axis.IsNearlyZero())
	{
		return;
	}

	// 탑다운에서의 이동: 카메라 기준이 아닌 월드 기준 (Yaw=0 고정 카메라)
	const FVector MoveDirection = FVector(Axis.Y, Axis.X, 0.0f).GetSafeNormal();

	if (!MoveDirection.IsNearlyZero())
	{
		// 캐릭터를 이동 방향으로 회전
		const FRotator TargetRotation = MoveDirection.Rotation();
		const float DT = GetWorld()->GetDeltaSeconds();
		const FRotator SmoothedRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DT, 10.0f);
		SetActorRotation(SmoothedRotation);

		// CharacterMovement를 통한 이동
		AddMovementInput(MoveDirection, 1.0f);
	}
}

void ASArenaCharacter::HandleZoom(const FInputActionValue& Value)
{
	const float ZoomDelta = Value.Get<float>();

	if (FMath::IsNearlyZero(ZoomDelta))
	{
		return;
	}

	// HRBCameraComponent에서 활성 카메라 모드를 가져와 줌 적용
	if (HRBCameraComp)
	{
		if (UHRBCameraMode_TopDown* TopDownMode = Cast<UHRBCameraMode_TopDown>(HRBCameraComp->GetActiveCameraMode()))
		{
			TopDownMode->ApplyZoomInput(ZoomDelta);
		}
	}
}
