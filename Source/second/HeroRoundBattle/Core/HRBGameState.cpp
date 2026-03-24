#include "HeroRoundBattle/Core/HRBGameState.h"
#include "Net/UnrealNetwork.h"

AHRBGameState::AHRBGameState()
{
}

void AHRBGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHRBGameState, RoundPhase);
	DOREPLIFETIME(AHRBGameState, CurrentRound);
	DOREPLIFETIME(AHRBGameState, MaxRounds);
	DOREPLIFETIME(AHRBGameState, ScorePlayer0);
	DOREPLIFETIME(AHRBGameState, ScorePlayer1);
	DOREPLIFETIME(AHRBGameState, RoundTimeRemaining);
	DOREPLIFETIME(AHRBGameState, WinnerPlayerIndex);
}

void AHRBGameState::OnRep_RoundPhase()
{
	OnRoundPhaseChanged.Broadcast(RoundPhase);
}

void AHRBGameState::OnRep_CurrentRound()
{
	OnRoundNumberChanged.Broadcast(CurrentRound);
}
