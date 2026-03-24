#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HeroRoundBattle/HRBTypes.h"
#include "HRBGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundPhaseChanged, ERoundPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundNumberChanged, int32, NewRound);

UCLASS()
class SECOND_API AHRBGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AHRBGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_RoundPhase, BlueprintReadOnly)
	ERoundPhase RoundPhase = ERoundPhase::WaitingForPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentRound, BlueprintReadOnly)
	int32 CurrentRound = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 MaxRounds = 10;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 ScorePlayer0 = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 ScorePlayer1 = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float RoundTimeRemaining = 60.f;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 WinnerPlayerIndex = -1;

	UPROPERTY(BlueprintAssignable)
	FOnRoundPhaseChanged OnRoundPhaseChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRoundNumberChanged OnRoundNumberChanged;

protected:
	UFUNCTION()
	void OnRep_RoundPhase();

	UFUNCTION()
	void OnRep_CurrentRound();
};
