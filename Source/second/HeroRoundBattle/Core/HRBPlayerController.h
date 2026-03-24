#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HRBPlayerController.generated.h"

class AHRBHeroCharacter;
class UInputAction;
class UInputMappingContext;

UCLASS()
class SECOND_API AHRBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AHRBPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	// Get this player's heroes
	TArray<AHRBHeroCharacter*> GetMyHeroes() const;

protected:
	// --- RTS Camera ---
	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraHeight = 1500.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraPitch = -60.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraScrollSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraEdgePanSpeed = 800.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraMinHeight = 500.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraMaxHeight = 3000.f;

	FVector CameraLocation;

	// --- Selection ---
	UPROPERTY()
	TArray<TWeakObjectPtr<AHRBHeroCharacter>> SelectedHeroes;

	// --- Input ---
	void HandleLeftClick();
	void HandleRightClick();
	void HandleSelectHero1();
	void HandleSelectHero2();
	void HandleSelectHero3();
	void HandleSelectAll();

	// --- Server RPCs ---
	UFUNCTION(Server, Reliable)
	void Server_CommandMoveTo(AHRBHeroCharacter* Hero, FVector Destination);

	UFUNCTION(Server, Reliable)
	void Server_CommandAttack(AHRBHeroCharacter* Hero, AHRBHeroCharacter* Target);

	UFUNCTION(Server, Reliable)
	void Server_CommandStop(AHRBHeroCharacter* Hero);

	// --- Helpers ---
	bool GetMouseWorldLocation(FVector& OutLocation) const;
	AHRBHeroCharacter* GetHeroUnderCursor() const;
	void SelectHero(AHRBHeroCharacter* Hero);
	void SelectHeroByIndex(int32 Index);
	void SelectAllHeroes();

	int32 GetMyPlayerIndex() const;

	// Enhanced Input
	UPROPERTY()
	TObjectPtr<UInputAction> IA_LeftClick;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_RightClick;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_Select1;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_Select2;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_Select3;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_SelectAll;

	UPROPERTY()
	TObjectPtr<UInputAction> IA_Scroll;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> IMC_RTS;

	void HandleScroll(const struct FInputActionValue& Value);
};
