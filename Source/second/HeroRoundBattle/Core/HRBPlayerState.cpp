#include "HeroRoundBattle/Core/HRBPlayerState.h"
#include "Net/UnrealNetwork.h"

AHRBPlayerState::AHRBPlayerState()
{
}

void AHRBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHRBPlayerState, SelectedRace);
	DOREPLIFETIME(AHRBPlayerState, PlayerIndex);
	DOREPLIFETIME(AHRBPlayerState, RoundWins);
	DOREPLIFETIME(AHRBPlayerState, bReady);
}
