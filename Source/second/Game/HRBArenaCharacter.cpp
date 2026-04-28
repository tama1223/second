// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Game/HRBArenaCharacter.h"

#include "Camera/HRBCameraComponent.h"
#include "Camera/HRBCameraMode.h"
#include "Camera/HRBCameraMode_TopDown.h"
#include "Character/HRBPawnData.h"
#include "GameMode/HRBGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBArenaCharacter)

AHRBArenaCharacter::AHRBArenaCharacter()
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

	// 카메라 폰은 server-authoritative movement 비활성화 — 클라 로컬에서 자유 이동
	// (RMB+WASD 카메라 팬을 위해 AddActorWorldOffset이 server reconciliation에 의해 되돌려지지 않게)
	SetReplicates(false);
	SetReplicateMovement(false);
	GetCharacterMovement()->SetIsReplicated(false);
	GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->GravityScale = 0.0f;

	// 카메라 폰은 MOVE_Flying 비행 카메라이므로 캡슐 충돌이 불필요.
	// 충돌을 끄지 않으면 영웅들의 CharacterMovement에서 "stuck and failed to move" 경고가 반복 발생.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// HRBCameraComponent 생성 (Lyra 패턴)
	HRBCameraComp = CreateDefaultSubobject<UHRBCameraComponent>(TEXT("HRBCamera"));
	HRBCameraComp->SetupAttachment(GetRootComponent());
	HRBCameraComp->SetUsingAbsoluteLocation(true);
	HRBCameraComp->SetUsingAbsoluteRotation(true);
	HRBCameraComp->bUsePawnControlRotation = false;

	// 기본 카메라 모드 폴백 — PawnData에 DefaultCameraMode 미설정 시 TopDown 사용
	DefaultCameraModeClass = UHRBCameraMode_TopDown::StaticClass();

	// Auto-possess 비활성화 - GameMode의 RestartPlayer 흐름에만 의존
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
}

void AHRBArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 카메라 모드 결정 델리게이트 바인딩
	if (HRBCameraComp)
	{
		HRBCameraComp->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
	}
}

const UHRBPawnData* AHRBArenaCharacter::GetCurrentPawnData() const
{
	if (const AController* MyController = GetController())
	{
		if (const UWorld* World = GetWorld())
		{
			if (const AHRBGameMode* GM = Cast<AHRBGameMode>(World->GetAuthGameMode()))
			{
				return GM->GetPawnDataForController(MyController);
			}
		}
	}
	return nullptr;
}

TSubclassOf<UHRBCameraMode> AHRBArenaCharacter::DetermineCameraMode() const
{
	// PawnData에서 DefaultCameraMode를 우선 사용
	if (const UHRBPawnData* PawnData = GetCurrentPawnData())
	{
		if (PawnData->DefaultCameraMode)
		{
			return PawnData->DefaultCameraMode;
		}
	}

	// 폴백: 캐릭터에 직접 설정된 DefaultCameraModeClass
	return DefaultCameraModeClass;
}

void AHRBArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AHRBArenaCharacter::HandleMove);
		EIC->BindAction(IA_Zoom, ETriggerEvent::Triggered, this, &AHRBArenaCharacter::HandleZoom);
	}
}

void AHRBArenaCharacter::HandleMove(const FInputActionValue& Value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (!PC)
	{
		return;
	}

	// RMB 홀드 중일 때만 카메라 팬으로 동작 (RMB 단발 클릭은 IA_MoveCommand로 빠져나감)
	if (!PC->IsInputKeyDown(EKeys::RightMouseButton))
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();
	if (Axis.IsNearlyZero())
	{
		return;
	}

	// 월드 기준 팬: Swizzle/Negate로 만든 축을 그대로 XY로 투영
	// (IMC_Arena가 W=+Y, S=-Y, D=+X, A=-X 로 매핑하므로 Axis.Y를 World X로, Axis.X를 World Y로 보낸다)
	const FVector PanDirection = FVector(Axis.Y, Axis.X, 0.0f);
	const float DT = GetWorld()->GetDeltaSeconds();
	const FVector Delta = PanDirection * PanSpeed * DT;

	// Sweep 없이 텔레포트식 이동 (카메라 폰은 가벼운 이동으로 충분)
	AddActorWorldOffset(Delta, /*bSweep=*/ false, nullptr, ETeleportType::TeleportPhysics);
}

void AHRBArenaCharacter::HandleZoom(const FInputActionValue& Value)
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
