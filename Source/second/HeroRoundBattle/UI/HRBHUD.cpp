#include "HeroRoundBattle/UI/HRBHUD.h"
#include "HeroRoundBattle/Core/HRBGameState.h"
#include "HeroRoundBattle/Core/HRBPlayerController.h"
#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"

void AHRBHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AHRBHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawHeroHPBars();
	DrawScoreboard();
	DrawRoundInfo();
}

void AHRBHUD::DrawHeroHPBars()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	for (TActorIterator<AHRBHeroCharacter> It(GetWorld()); It; ++It)
	{
		AHRBHeroCharacter* Hero = *It;
		if (!Hero || Hero->IsDead()) continue;

		FVector WorldPos = Hero->GetActorLocation() + FVector(0, 0, 120.f);
		FVector2D ScreenPos;
		if (PC->ProjectWorldLocationToScreen(WorldPos, ScreenPos))
		{
			DrawHPBar(Hero, ScreenPos);
		}
	}
}

void AHRBHUD::DrawHPBar(AHRBHeroCharacter* Hero, const FVector2D& ScreenPos)
{
	const float BarWidth = 60.f;
	const float BarHeight = 8.f;
	const float X = ScreenPos.X - BarWidth * 0.5f;
	const float Y = ScreenPos.Y - BarHeight;

	float HPPercent = (Hero->GetMaxHP() > 0) ? Hero->GetCurrentHP() / Hero->GetMaxHP() : 0.f;
	HPPercent = FMath::Clamp(HPPercent, 0.f, 1.f);

	// Background
	DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f), X - 1, Y - 1, BarWidth + 2, BarHeight + 2);

	// HP fill - green for allies, red for enemies
	AHRBPlayerController* MyPC = Cast<AHRBPlayerController>(GetOwningPlayerController());
	bool bIsAlly = MyPC && (Hero->GetOwningPlayer() == Cast<AHRBPlayerController>(MyPC)->GetMyHeroes().Num()); // simplified

	FLinearColor BarColor = (Hero->GetOwningPlayer() == 0) ? FLinearColor(0.2f, 0.7f, 1.f) : FLinearColor(1.f, 0.3f, 0.3f);
	DrawRect(BarColor, X, Y, BarWidth * HPPercent, BarHeight);

	// Hero name
	FString Name = Hero->GetHeroDef().DisplayName.ToString();
	DrawText(Name, FColor::White, X, Y - 14.f, nullptr, 0.8f);

	// Level
	FString LevelText = FString::Printf(TEXT("Lv.%d"), Hero->GetHeroLevel());
	DrawText(LevelText, FColor::Yellow, X + BarWidth - 20.f, Y - 14.f, nullptr, 0.7f);
}

void AHRBHUD::DrawScoreboard()
{
	AHRBGameState* GS = GetWorld()->GetGameState<AHRBGameState>();
	if (!GS) return;

	float CenterX = Canvas->SizeX * 0.5f;

	// Score display at top center
	FString ScoreText = FString::Printf(TEXT("%d  -  %d"), GS->ScorePlayer0, GS->ScorePlayer1);
	DrawText(ScoreText, FColor::White, CenterX - 30.f, 20.f, nullptr, 2.f);

	// Player labels
	DrawText(TEXT("Human"), FColor(100, 150, 255), CenterX - 100.f, 20.f, nullptr, 1.2f);
	DrawText(TEXT("Undead"), FColor(255, 100, 100), CenterX + 50.f, 20.f, nullptr, 1.2f);
}

void AHRBHUD::DrawRoundInfo()
{
	AHRBGameState* GS = GetWorld()->GetGameState<AHRBGameState>();
	if (!GS) return;

	float CenterX = Canvas->SizeX * 0.5f;

	// Round number
	FString RoundText = FString::Printf(TEXT("Round %d / %d"), GS->CurrentRound, GS->MaxRounds);
	DrawText(RoundText, FColor::White, CenterX - 40.f, 55.f, nullptr, 1.f);

	// Phase info
	FString PhaseText = GetPhaseText(GS->RoundPhase);
	DrawText(PhaseText, FColor::Yellow, CenterX - 60.f, 75.f, nullptr, 1.2f);

	// Timer (during active round or countdown)
	if (GS->RoundPhase == ERoundPhase::RoundActive || GS->RoundPhase == ERoundPhase::RoundCountdown)
	{
		int32 TimeInt = FMath::CeilToInt(GS->RoundTimeRemaining);
		FString TimeText = FString::Printf(TEXT("%d"), TimeInt);
		DrawText(TimeText, FColor::White, CenterX - 10.f, 100.f, nullptr, 2.0f);
	}

	// Game over
	if (GS->RoundPhase == ERoundPhase::GameOver)
	{
		FString WinnerText;
		if (GS->WinnerPlayerIndex == 0)
			WinnerText = TEXT("HUMAN WINS!");
		else if (GS->WinnerPlayerIndex == 1)
			WinnerText = TEXT("UNDEAD WINS!");
		else
			WinnerText = TEXT("DRAW!");

		DrawText(WinnerText, FColor::Yellow, CenterX - 80.f, Canvas->SizeY * 0.4f, nullptr, 3.f);
	}
}

FString AHRBHUD::GetPhaseText(ERoundPhase Phase) const
{
	switch (Phase)
	{
	case ERoundPhase::WaitingForPlayers: return TEXT("Waiting for Players...");
	case ERoundPhase::RaceSelect: return TEXT("Select Your Race");
	case ERoundPhase::RoundCountdown: return TEXT("Get Ready!");
	case ERoundPhase::RoundActive: return TEXT("FIGHT!");
	case ERoundPhase::RoundEnd: return TEXT("Round Over");
	case ERoundPhase::GameOver: return TEXT("Game Over");
	default: return TEXT("");
	}
}
