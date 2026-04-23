// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HRBArenaCharacter.generated.h"

class UHRBCameraComponent;
class UHRBCameraMode;
class UHRBCameraMode_TopDown;
class UHRBPawnData;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * AHRBArenaCharacter
 *
 * 아레나 탑다운 시점 전용 캐릭터.
 * HRBCameraComponent를 사용하여 카메라 모드 기반 뷰를 제공한다.
 */
UCLASS()
class SECOND_API AHRBArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHRBArenaCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	/** HRBCameraComponent (Lyra 패턴) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHRBCameraComponent> HRBCameraComp;

	/** 카메라 모드 클래스 폴백 (PawnData에 설정이 없을 때 사용) */
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
	/** PawnData에서 카메라 모드를 조회, 폴백으로 DefaultCameraModeClass 사용 */
	TSubclassOf<UHRBCameraMode> DetermineCameraMode() const;

	/** 현재 Pawn에 대한 PawnData 조회 (GameMode 경유) */
	const UHRBPawnData* GetCurrentPawnData() const;

	// ---------- Input Handlers ----------
	void HandleMove(const FInputActionValue& Value);
	void HandleZoom(const FInputActionValue& Value);
};
