// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/HRBTestPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Engine/StaticMesh.h"

AHRBTestPawn::AHRBTestPawn()
{
	PrimaryActorTick.bCanEverTick = false;


	// 빈 루트
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);


	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(SceneRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (CubeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMesh.Object);
		MeshComp->SetWorldScale3D(FVector(0.5f));
	}
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionProfileName(TEXT("Pawn"));
	MeshComp->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));


	// Camera – 3rd person offset
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SceneRoot);
	CameraComp->SetRelativeLocation(FVector(-400.f, 0.f, 200.f));
	CameraComp->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	// Auto-possess
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AHRBTestPawn::BeginPlay()
{
	Super::BeginPlay();

}

void AHRBTestPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ---- Create Input Actions at runtime (no asset files needed) ----

	// Move (WASD) – Axis2D
	IA_Move = NewObject<UInputAction>(this, TEXT("IA_Move"));
	IA_Move->ValueType = EInputActionValueType::Axis2D;

	// VerticalMove (Space / Ctrl) – Axis1D
	IA_VerticalMove = NewObject<UInputAction>(this, TEXT("IA_VerticalMove"));
	IA_VerticalMove->ValueType = EInputActionValueType::Axis1D;

	// Rotate (QE / Mouse X) – Axis1D
	IA_Rotate = NewObject<UInputAction>(this, TEXT("IA_Rotate"));
	IA_Rotate->ValueType = EInputActionValueType::Axis1D;

	// ---- Create Mapping Context and bind keys ----
	IMC_Default = NewObject<UInputMappingContext>(this, TEXT("IMC_Default"));

	// --- Move ---
	{
		FEnhancedActionKeyMapping& W = IMC_Default->MapKey(IA_Move, EKeys::W);
		auto* SwizzleMod = NewObject<UInputModifierSwizzleAxis>(this);
		SwizzleMod->Order = EInputAxisSwizzle::YXZ;
		W.Modifiers.Add(SwizzleMod);
	}
	{
		FEnhancedActionKeyMapping& S = IMC_Default->MapKey(IA_Move, EKeys::S);
		auto* SwizzleMod = NewObject<UInputModifierSwizzleAxis>(this);
		SwizzleMod->Order = EInputAxisSwizzle::YXZ;
		S.Modifiers.Add(SwizzleMod);
		auto* Negate = NewObject<UInputModifierNegate>(this);
		S.Modifiers.Add(Negate);
	}
	{
		IMC_Default->MapKey(IA_Move, EKeys::D);
	}
	{
		FEnhancedActionKeyMapping& A = IMC_Default->MapKey(IA_Move, EKeys::A);
		auto* Negate = NewObject<UInputModifierNegate>(this);
		A.Modifiers.Add(Negate);
	}

	// --- Vertical Move ---
	{
		IMC_Default->MapKey(IA_VerticalMove, EKeys::SpaceBar);
	}
	{
		FEnhancedActionKeyMapping& Ctrl = IMC_Default->MapKey(IA_VerticalMove, EKeys::LeftControl);
		auto* Negate = NewObject<UInputModifierNegate>(this);
		Ctrl.Modifiers.Add(Negate);
	}

	// --- Rotate ---
	{
		IMC_Default->MapKey(IA_Rotate, EKeys::E);
	}
	{
		FEnhancedActionKeyMapping& Q = IMC_Default->MapKey(IA_Rotate, EKeys::Q);
		auto* Negate = NewObject<UInputModifierNegate>(this);
		Q.Modifiers.Add(Negate);
	}
	{
		IMC_Default->MapKey(IA_Rotate, EKeys::MouseX);
	}

	// ---- Register context with the local player ----
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Default, 0);
		}
	}

	if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AHRBTestPawn::HandleMove);
		EIC->BindAction(IA_VerticalMove, ETriggerEvent::Triggered, this, &AHRBTestPawn::HandleVerticalMove);
		EIC->BindAction(IA_Rotate, ETriggerEvent::Triggered, this, &AHRBTestPawn::HandleRotate);
	}
}

// ---- Handlers ----

void AHRBTestPawn::HandleMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	// const FRotator Yaw(0.f, MeshComp->GetComponentRotation().Yaw, 0.f);
	const FRotator Yaw(0.f, GetActorRotation().Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
	const FVector RightDirection   = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);

	const float DT = GetWorld()->GetDeltaSeconds();
	UE_LOG(LogTemp, Log, TEXT("HandleMove :: DT: %f"), DT);
	// AddActorWorldOffset((FVector(Axis.Y, Axis.X, 0.f)) * MoveSpeed * DT, true);
	// AddActorLocalOffset((FVector(Axis.Y, Axis.X, 0.f)) * MoveSpeed * DT, true);

	const FVector MoveDirection = (ForwardDirection * Axis.Y + RightDirection * Axis.X).GetSafeNormal();
	const FRotator TargetRotation = MoveDirection.Rotation();
    const FRotator SmoothedRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DT, 10.f);
	SetActorRotation(SmoothedRotation);

	AddActorWorldOffset((ForwardDirection * Axis.Y + RightDirection * Axis.X) * MoveSpeed * DT, true);
}

void AHRBTestPawn::HandleVerticalMove(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();
	const float DT = GetWorld()->GetDeltaSeconds();
	AddActorWorldOffset(FVector::UpVector * Axis * VerticalSpeed * DT, true);
}

void AHRBTestPawn::HandleRotate(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();
	const float DT = GetWorld()->GetDeltaSeconds();
	// MeshComp->AddWorldRotation(FRotator(0.f, Axis * RotationSpeed * DT, 0.f));
	AddActorWorldRotation(FRotator(0.f, Axis * RotationSpeed * DT, 0.f));

	UE_LOG(LogTemp, Log, TEXT("HandleRotate :: Axis: %f"), Axis);
}
