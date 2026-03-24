#include "HeroRoundBattle/UI/HRBHeroHPBar.h"
#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"

UHRBHeroHPBar::UHRBHeroHPBar()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawSize(FVector2D(100.f, 10.f));
	SetRelativeLocation(FVector(0.f, 0.f, 120.f));
}

void UHRBHeroHPBar::InitForHero(AHRBHeroCharacter* InHero)
{
	OwnerHero = InHero;
}
