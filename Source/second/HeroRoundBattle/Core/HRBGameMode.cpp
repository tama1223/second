#include "HeroRoundBattle/Core/HRBGameMode.h"
#include "HeroRoundBattle/Core/HRBGameState.h"
#include "HeroRoundBattle/Core/HRBPlayerState.h"
#include "HeroRoundBattle/Core/HRBPlayerController.h"
#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"
#include "HeroRoundBattle/UI/HRBHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"

AHRBGameMode::AHRBGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = nullptr; // RTS - no pawn to possess
	PlayerControllerClass = AHRBPlayerController::StaticClass();
	PlayerStateClass = AHRBPlayerState::StaticClass();
	GameStateClass = AHRBGameState::StaticClass();
	HUDClass = AHRBHUD::StaticClass();
}

void AHRBGameMode::BeginPlay()
{
	Super::BeginPlay();

	HRBGameState = GetGameState<AHRBGameState>();
	check(HRBGameState);

	HRBGameState->MaxRounds = MaxRounds;

	InitDefaultHeroPool();

	SetPhase(ERoundPhase::WaitingForPlayers);
}

void AHRBGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HRBGameState) return;

	switch (HRBGameState->RoundPhase)
	{
	case ERoundPhase::WaitingForPlayers:
		if (Players.Num() >= 2)
		{
			// Auto-assign races for MVP: Player0=Human, Player1=Undead
			if (AHRBPlayerState* PS0 = Players[0]->GetPlayerState<AHRBPlayerState>())
			{
				PS0->PlayerIndex = 0;
				PS0->SelectedRace = ERace::Human;
			}
			if (AHRBPlayerState* PS1 = Players[1]->GetPlayerState<AHRBPlayerState>())
			{
				PS1->PlayerIndex = 1;
				PS1->SelectedRace = ERace::Undead;
			}

			StartNextRound();
		}
		break;

	case ERoundPhase::RoundCountdown:
		PhaseTimer -= DeltaTime;
		HRBGameState->RoundTimeRemaining = PhaseTimer;
		if (PhaseTimer <= 0.f)
		{
			SetPhase(ERoundPhase::RoundActive);
			HRBGameState->RoundTimeRemaining = RoundTimeLimit;
		}
		break;

	case ERoundPhase::RoundActive:
		HRBGameState->RoundTimeRemaining -= DeltaTime;
		if (HRBGameState->RoundTimeRemaining <= 0.f)
		{
			DetermineRoundWinnerByHP();
		}
		break;

	case ERoundPhase::RoundEnd:
		PhaseTimer -= DeltaTime;
		if (PhaseTimer <= 0.f)
		{
			if (CurrentRound >= MaxRounds)
			{
				EndGame();
			}
			else
			{
				StartNextRound();
			}
		}
		break;

	default:
		break;
	}
}

void AHRBGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (Players.Num() < 2)
	{
		Players.Add(NewPlayer);
		UE_LOG(LogTemp, Log, TEXT("HRBGameMode: Player %d joined"), Players.Num() - 1);
	}
}

void AHRBGameMode::InitDefaultHeroPool()
{
	if (HeroPool.Num() > 0) return;

	// Human heroes
	{
		FHeroDefinition Def;
		Def.HeroID = FName("Paladin");
		Def.DisplayName = FText::FromString(TEXT("Paladin"));
		Def.Race = ERace::Human;
		Def.BaseStats.MaxHP = 120.f;
		Def.BaseStats.Attack = 12.f;
		Def.BaseStats.Defense = 4.f;
		Def.BaseStats.AttackSpeed = 0.8f;
		Def.BaseStats.MoveSpeed = 350.f;
		Def.BaseStats.AttackRange = 150.f;
		Def.BaseStats.AttackType = EHeroAttackType::Melee;
		Def.HeroColor = FLinearColor(0.2f, 0.4f, 1.f);
		HeroPool.Add(Def);
	}
	{
		FHeroDefinition Def;
		Def.HeroID = FName("Archmage");
		Def.DisplayName = FText::FromString(TEXT("Archmage"));
		Def.Race = ERace::Human;
		Def.BaseStats.MaxHP = 80.f;
		Def.BaseStats.Attack = 16.f;
		Def.BaseStats.Defense = 2.f;
		Def.BaseStats.AttackSpeed = 0.7f;
		Def.BaseStats.MoveSpeed = 380.f;
		Def.BaseStats.AttackRange = 500.f;
		Def.BaseStats.AttackType = EHeroAttackType::Ranged;
		Def.HeroColor = FLinearColor(0.5f, 0.5f, 1.f);
		HeroPool.Add(Def);
	}
	{
		FHeroDefinition Def;
		Def.HeroID = FName("MountainKing");
		Def.DisplayName = FText::FromString(TEXT("Mountain King"));
		Def.Race = ERace::Human;
		Def.BaseStats.MaxHP = 140.f;
		Def.BaseStats.Attack = 14.f;
		Def.BaseStats.Defense = 5.f;
		Def.BaseStats.AttackSpeed = 0.9f;
		Def.BaseStats.MoveSpeed = 330.f;
		Def.BaseStats.AttackRange = 150.f;
		Def.BaseStats.AttackType = EHeroAttackType::Melee;
		Def.HeroColor = FLinearColor(0.8f, 0.6f, 0.2f);
		HeroPool.Add(Def);
	}

	// Undead heroes
	{
		FHeroDefinition Def;
		Def.HeroID = FName("DeathKnight");
		Def.DisplayName = FText::FromString(TEXT("Death Knight"));
		Def.Race = ERace::Undead;
		Def.BaseStats.MaxHP = 130.f;
		Def.BaseStats.Attack = 13.f;
		Def.BaseStats.Defense = 3.f;
		Def.BaseStats.AttackSpeed = 0.85f;
		Def.BaseStats.MoveSpeed = 360.f;
		Def.BaseStats.AttackRange = 150.f;
		Def.BaseStats.AttackType = EHeroAttackType::Melee;
		Def.HeroColor = FLinearColor(0.1f, 0.8f, 0.1f);
		HeroPool.Add(Def);
	}
	{
		FHeroDefinition Def;
		Def.HeroID = FName("Lich");
		Def.DisplayName = FText::FromString(TEXT("Lich"));
		Def.Race = ERace::Undead;
		Def.BaseStats.MaxHP = 75.f;
		Def.BaseStats.Attack = 18.f;
		Def.BaseStats.Defense = 1.f;
		Def.BaseStats.AttackSpeed = 0.65f;
		Def.BaseStats.MoveSpeed = 370.f;
		Def.BaseStats.AttackRange = 550.f;
		Def.BaseStats.AttackType = EHeroAttackType::Ranged;
		Def.HeroColor = FLinearColor(0.3f, 0.3f, 0.9f);
		HeroPool.Add(Def);
	}
	{
		FHeroDefinition Def;
		Def.HeroID = FName("DreadLord");
		Def.DisplayName = FText::FromString(TEXT("Dread Lord"));
		Def.Race = ERace::Undead;
		Def.BaseStats.MaxHP = 110.f;
		Def.BaseStats.Attack = 15.f;
		Def.BaseStats.Defense = 3.f;
		Def.BaseStats.AttackSpeed = 0.75f;
		Def.BaseStats.MoveSpeed = 380.f;
		Def.BaseStats.AttackRange = 150.f;
		Def.BaseStats.AttackType = EHeroAttackType::Melee;
		Def.HeroColor = FLinearColor(0.6f, 0.1f, 0.1f);
		HeroPool.Add(Def);
	}
}

void AHRBGameMode::StartNextRound()
{
	CurrentRound++;
	HRBGameState->CurrentRound = CurrentRound;

	if (!bHeroesSpawned)
	{
		SpawnHeroesForPlayer(0, ERace::Human);
		SpawnHeroesForPlayer(1, ERace::Undead);
		bHeroesSpawned = true;
	}
	else
	{
		LevelUpAllHeroes();
	}

	// Reset heroes for new round
	for (AHRBHeroCharacter* Hero : AllHeroes)
	{
		if (Hero)
		{
			Hero->ResetForNewRound();

			// Reposition
			FVector SpawnLoc = GetSpawnLocation(Hero->GetOwningPlayer(), Hero->GetHeroIndex());
			Hero->SetActorLocation(SpawnLoc);
			Hero->GetCharacterMovement()->StopActiveMovement();
		}
	}

	// Countdown phase
	PhaseTimer = CountdownDuration;
	SetPhase(ERoundPhase::RoundCountdown);
}

void AHRBGameMode::SpawnHeroesForPlayer(int32 PlayerIdx, ERace Race)
{
	TArray<FHeroDefinition> RaceHeroes;
	for (const FHeroDefinition& Def : HeroPool)
	{
		if (Def.Race == Race)
		{
			RaceHeroes.Add(Def);
		}
	}

	int32 Count = FMath::Min(HeroesPerPlayer, RaceHeroes.Num());
	for (int32 i = 0; i < Count; i++)
	{
		FVector SpawnLoc = GetSpawnLocation(PlayerIdx, i);
		FRotator SpawnRot = (PlayerIdx == 0) ? FRotator(0, 0, 0) : FRotator(0, 180, 0);

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AHRBHeroCharacter* Hero = GetWorld()->SpawnActor<AHRBHeroCharacter>(AHRBHeroCharacter::StaticClass(), SpawnLoc, SpawnRot, Params);
		if (Hero)
		{
			Hero->InitHero(RaceHeroes[i], PlayerIdx, i);
			Hero->OnDied.AddDynamic(this, &AHRBGameMode::OnHeroDied);
			AllHeroes.Add(Hero);
		}
	}
}

void AHRBGameMode::LevelUpAllHeroes()
{
	for (AHRBHeroCharacter* Hero : AllHeroes)
	{
		if (Hero)
		{
			Hero->ApplyLevelUp(CurrentRound);
		}
	}
}

FVector AHRBGameMode::GetSpawnLocation(int32 PlayerIdx, int32 HeroIdx) const
{
	// Player 0 on left side, Player 1 on right side
	float XOffset = (PlayerIdx == 0) ? -600.f : 600.f;
	float YOffset = (HeroIdx - 1) * 200.f; // -200, 0, +200

	return FVector(XOffset, YOffset, 100.f);
}

void AHRBGameMode::OnHeroDied(AHRBHeroCharacter* DeadHero)
{
	CheckRoundWinCondition();
}

void AHRBGameMode::CheckRoundWinCondition()
{
	if (HRBGameState->RoundPhase != ERoundPhase::RoundActive) return;

	int32 AliveCount[2] = {0, 0};

	for (AHRBHeroCharacter* Hero : AllHeroes)
	{
		if (Hero && !Hero->IsDead())
		{
			int32 Idx = Hero->GetOwningPlayer();
			if (Idx >= 0 && Idx < 2)
			{
				AliveCount[Idx]++;
			}
		}
	}

	if (AliveCount[0] == 0 && AliveCount[1] == 0)
	{
		// Draw - no score change
		EndRound(-1);
	}
	else if (AliveCount[0] == 0)
	{
		EndRound(1);
	}
	else if (AliveCount[1] == 0)
	{
		EndRound(0);
	}
}

void AHRBGameMode::DetermineRoundWinnerByHP()
{
	// Timeout: compare total HP percentages
	float TotalHPPercent[2] = {0.f, 0.f};
	int32 HeroCount[2] = {0, 0};

	for (AHRBHeroCharacter* Hero : AllHeroes)
	{
		if (Hero && !Hero->IsDead())
		{
			int32 Idx = Hero->GetOwningPlayer();
			if (Idx >= 0 && Idx < 2)
			{
				TotalHPPercent[Idx] += Hero->GetCurrentHP() / Hero->GetMaxHP();
				HeroCount[Idx]++;
			}
		}
	}

	for (int i = 0; i < 2; i++)
	{
		if (HeroCount[i] > 0)
			TotalHPPercent[i] /= HeroCount[i];
	}

	if (TotalHPPercent[0] > TotalHPPercent[1])
		EndRound(0);
	else if (TotalHPPercent[1] > TotalHPPercent[0])
		EndRound(1);
	else
		EndRound(-1); // Draw
}

void AHRBGameMode::EndRound(int32 WinnerIdx)
{
	if (WinnerIdx == 0)
		HRBGameState->ScorePlayer0++;
	else if (WinnerIdx == 1)
		HRBGameState->ScorePlayer1++;

	// Stop all heroes
	for (AHRBHeroCharacter* Hero : AllHeroes)
	{
		if (Hero)
		{
			Hero->CommandStop();
		}
	}

	PhaseTimer = 3.f; // Brief pause before next round
	SetPhase(ERoundPhase::RoundEnd);

	UE_LOG(LogTemp, Log, TEXT("Round %d ended. Winner: Player %d. Score: %d - %d"),
		CurrentRound, WinnerIdx, HRBGameState->ScorePlayer0, HRBGameState->ScorePlayer1);
}

void AHRBGameMode::EndGame()
{
	if (HRBGameState->ScorePlayer0 > HRBGameState->ScorePlayer1)
		HRBGameState->WinnerPlayerIndex = 0;
	else if (HRBGameState->ScorePlayer1 > HRBGameState->ScorePlayer0)
		HRBGameState->WinnerPlayerIndex = 1;
	else
		HRBGameState->WinnerPlayerIndex = -1; // Draw

	SetPhase(ERoundPhase::GameOver);

	UE_LOG(LogTemp, Log, TEXT("Game Over! Winner: Player %d. Final Score: %d - %d"),
		HRBGameState->WinnerPlayerIndex, HRBGameState->ScorePlayer0, HRBGameState->ScorePlayer1);
}

void AHRBGameMode::SetPhase(ERoundPhase NewPhase)
{
	if (HRBGameState)
	{
		HRBGameState->RoundPhase = NewPhase;
		// Force OnRep for listen server
		HRBGameState->OnRoundPhaseChanged.Broadcast(NewPhase);
	}
}

void AHRBGameMode::CleanupHeroes()
{
	for (AHRBHeroCharacter* Hero : AllHeroes)
	{
		if (Hero)
		{
			Hero->Destroy();
		}
	}
	AllHeroes.Empty();
	bHeroesSpawned = false;
}
