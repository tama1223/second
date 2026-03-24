#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HRBHeroHPBar.generated.h"

class AHRBHeroCharacter;

// Simple world-space HP bar that attaches above a hero
UCLASS()
class SECOND_API UHRBHeroHPBar : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UHRBHeroHPBar();

	void InitForHero(AHRBHeroCharacter* InHero);

protected:
	UPROPERTY()
	TWeakObjectPtr<AHRBHeroCharacter> OwnerHero;
};
