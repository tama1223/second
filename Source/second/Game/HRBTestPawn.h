// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HRBTestPawn.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class SECOND_API AHRBTestPawn : public APawn
{
	GENERATED_BODY()

public:
	AHRBTestPawn();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	// ---------- Components ----------
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComp;

	// ---------- Input ----------
	UPROPERTY()
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_VerticalMove;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_Rotate;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> IMC_Default;

	// ---------- Tuning ----------
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float VerticalSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotationSpeed = 90.f;

	// ---------- Input Handlers ----------
	void HandleMove(const FInputActionValue& Value);
	void HandleVerticalMove(const FInputActionValue& Value);
	void HandleRotate(const FInputActionValue& Value);
};
