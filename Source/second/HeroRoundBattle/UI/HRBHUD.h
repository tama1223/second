#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HeroRoundBattle/HRBTypes.h"
#include "HRBHUD.generated.h"

class AHRBHeroCharacter;
class AHRBGameState;

UCLASS()
class SECOND_API AHRBHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

protected:
	void DrawHeroHPBars();
	void DrawScoreboard();
	void DrawRoundInfo();

	void DrawHPBar(AHRBHeroCharacter* Hero, const FVector2D& ScreenPos);

	FString GetPhaseText(ERoundPhase Phase) const;
};
