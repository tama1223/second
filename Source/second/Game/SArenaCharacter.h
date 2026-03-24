// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SArenaCharacter.generated.h"

class UHRBCameraComponent;
class UHRBCameraMode;
class UHRBCameraMode_TopDown;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * ASArenaCharacter
 *
 * 아레나 탑다운 시점 전용 캐릭터.
 * HRBCameraComponent를 사용하여 카메라 모드 기반 뷰를 제공한다.
 */
UCLASS()
class SECOND_API ASArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASArenaCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	/** HRBCameraComponent (Lyra 패턴) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHRBCameraComponent> HRBCameraComp;

	/** 카메라 모드 클래스 (에디터에서 설정 또는 기본값) */
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UHRBCameraMode> DefaultCameraModeClass;

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

	// ---------- Callbacks ----------
	TSubclassOf<UHRBCameraMode> DetermineCameraMode() const;

	// ---------- Input Handlers ----------
	void HandleMove(const FInputActionValue& Value);
	void HandleZoom(const FInputActionValue& Value);
};
