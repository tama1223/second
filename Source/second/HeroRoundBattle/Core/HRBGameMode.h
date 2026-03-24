#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HeroRoundBattle/HRBTypes.h"
#include "HRBGameMode.generated.h"

class AHRBHeroCharacter;
class AHRBGameState;
class AHRBAIController;

UCLASS()
class SECOND_API AHRBGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHRBGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Hero definitions per race
	UPROPERTY(EditAnywhere, Category = "Heroes")
	TArray<FHeroDefinition> HeroPool;

	UPROPERTY(EditAnywhere, Category = "Round")
	float RoundTimeLimit = 60.f;

	UPROPERTY(EditAnywhere, Category = "Round")
	float CountdownDuration = 5.f;

	UPROPERTY(EditAnywhere, Category = "Round")
	int32 MaxRounds = 10;

	UPROPERTY(EditAnywhere, Category = "Round")
	int32 HeroesPerPlayer = 3;

	// AI opponent mode (1v1 vs AI)
	UPROPERTY(EditAnywhere, Category = "AI")
	bool bUseAIOpponent = true;

	void OnHeroDied(AHRBHeroCharacter* DeadHero);

protected:
	void InitDefaultHeroPool();

	void SetPhase(ERoundPhase NewPhase);
	void StartNextRound();
	void SpawnHeroesForPlayer(int32 PlayerIdx, ERace Race);
	void LevelUpAllHeroes();
	void CheckRoundWinCondition();
	void DetermineRoundWinnerByHP();
	void EndRound(int32 WinnerIdx);
	void EndGame();
	void CleanupHeroes();

	// AI support
	void SpawnAIControllersForPlayer(int32 PlayerIdx);
	void CleanupAIControllers();

	UPROPERTY()
	TArray<AHRBHeroCharacter*> AllHeroes;

	UPROPERTY()
	TArray<AHRBAIController*> AIControllers;

	UPROPERTY()
	TArray<APlayerController*> Players;

	AHRBGameState* HRBGameState = nullptr;

	float PhaseTimer = 0.f;
	int32 CurrentRound = 0;
	bool bHeroesSpawned = false;

	// Spawn positions
	FVector GetSpawnLocation(int32 PlayerIdx, int32 HeroIdx) const;
};
