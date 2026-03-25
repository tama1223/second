// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HRBPlayerController.generated.h"

class AHRBHeroCharacter;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * AHRBPlayerController
 *
 * 영웅 선택 및 명령을 담당하는 PlayerController.
 * - 좌클릭: 영웅 선택 (라인 트레이스)
 * - 1/2/3 키: 영웅 인덱스 전환
 * - 좌클릭 드래그: 범위 선택
 */
UCLASS()
class SECOND_API AHRBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AHRBPlayerController();

	/** 스폰된 영웅 등록 (GameMode에서 호출) */
	void RegisterHero(AHRBHeroCharacter* Hero);

	/** 등록된 영웅 목록 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|Selection")
	TArray<TObjectPtr<AHRBHeroCharacter>> Heroes;

	/** 현재 선택된 영웅들 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|Selection")
	TArray<TObjectPtr<AHRBHeroCharacter>> SelectedHeroes;

	// 드래그 선택 상태 (HUD에서 사각형 그리기 위해 public)
	bool bIsDragging = false;
	FVector2D DragStartScreen;
	FVector2D DragCurrentScreen;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

private:
	// ---------- Input Actions ----------
	UPROPERTY()
	TObjectPtr<UInputAction> IA_Select;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_SelectHero1;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_SelectHero2;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_SelectHero3;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> IMC_Selection;

	// ---------- Selection Logic ----------
	/** 단일 선택 (라인 트레이스) */
	void SelectHeroUnderCursor();

	/** 인덱스로 영웅 선택 */
	void SelectHeroByIndex(int32 Index);

	/** 모든 영웅 선택 해제 */
	void ClearSelection();

	/** 드래그 영역 내 영웅 선택 */
	void SelectHeroesInDragRect();

	// ---------- Input Handlers ----------
	void HandleSelectStarted(const FInputActionValue& Value);
	void HandleSelectCompleted(const FInputActionValue& Value);
	void HandleSelectHero1(const FInputActionValue& Value);
	void HandleSelectHero2(const FInputActionValue& Value);
	void HandleSelectHero3(const FInputActionValue& Value);

	// ---------- Drag ----------
	/** 드래그 판정 임계값 (픽셀) */
	float DragThreshold = 10.0f;
};
