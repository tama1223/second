#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HRBAIController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class AHRBHeroCharacter;
class AHRBGameState;

UCLASS()
class SECOND_API AHRBAIController : public AAIController
{
	GENERATED_BODY()

public:
	AHRBAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	/** Get the hero this AI controls */
	AHRBHeroCharacter* GetControlledHero() const;

	/** Get all living enemy heroes */
	TArray<AHRBHeroCharacter*> GetEnemyHeroes() const;

	/** AI target selection: focus fire on weakest enemy */
	AHRBHeroCharacter* FindBestTarget() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> HeroBehaviorTree;

	// Fallback tick-based AI when BT asset not found
	bool bUseFallbackAI = false;
	float FallbackDecisionTimer = 0.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float FallbackDecisionInterval = 0.8f;

	void TickFallbackAI(float DeltaTime);
};
