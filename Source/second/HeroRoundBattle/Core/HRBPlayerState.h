#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HeroRoundBattle/HRBTypes.h"
#include "HRBPlayerState.generated.h"

UCLASS()
class SECOND_API AHRBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AHRBPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	ERace SelectedRace = ERace::Human;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 PlayerIndex = -1;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RoundWins = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bReady = false;
};
