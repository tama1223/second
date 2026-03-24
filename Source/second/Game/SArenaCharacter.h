// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SArenaCharacter.generated.h"

class UCameraComponent;
class USCameraMode_TopDown;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * ASArenaCharacter
 *
 * 아레나 탑다운 시점 전용 캐릭터.
 * 탑다운 카메라 모드와 마우스 휠 줌을 내장한다.
 */
UCLASS()
class SECOND_API ASArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASArenaCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	// ---------- Components ----------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComp;

	// ---------- Camera Mode ----------
	UPROPERTY()
	TObjectPtr<USCameraMode_TopDown> CameraMode;

	// ---------- Input ----------
	UPROPERTY()
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_Zoom;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> IMC_Arena;

	// ---------- Tuning ----------
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 600.0f;

	// ---------- Input Handlers ----------
	void HandleMove(const FInputActionValue& Value);
	void HandleZoom(const FInputActionValue& Value);
};
