// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "GameMode/HRBGameMode.h"

#include "GameMode/HRBExperienceDefinition.h"
#include "GameMode/HRBExperienceManagerComponent.h"
#include "Character/HRBPawnData.h"
#include "Hero/HRBHeroCharacter.h"
#include "Hero/HRBEnemyHeroCharacter.h"
#include "Player/HRBPlayerController.h"
#include "Player/HRBSelectionHUD.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBGameMode)

AHRBGameMode::AHRBGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Pawn 스폰을 Experience 로드 완료까지 지연시키기 위해 기본 PawnClass를 nullptr로 설정
	DefaultPawnClass = nullptr;

	// PlayerController와 HUD 클래스 설정
	PlayerControllerClass = AHRBPlayerController::StaticClass();
	HUDClass = AHRBSelectionHUD::StaticClass();

	// 기본 영웅 스폰 위치 (삼각형 배치)
	HeroSpawnLocations.Add(FVector(0.0f, 0.0f, 100.0f));
	HeroSpawnLocations.Add(FVector(-200.0f, -200.0f, 100.0f));
	HeroSpawnLocations.Add(FVector(-200.0f, 200.0f, 100.0f));

	// 기본 영웅 클래스
	HeroCharacterClass = AHRBHeroCharacter::StaticClass();

	// 기본 적 영웅 클래스
	EnemyHeroCharacterClass = AHRBEnemyHeroCharacter::StaticClass();

	// 적 스폰 위치 (반대편)
	EnemySpawnLocations.Add(FVector(300.0f, 0.0f, 100.0f));
	EnemySpawnLocations.Add(FVector(300.0f, -300.0f, 100.0f));
	EnemySpawnLocations.Add(FVector(300.0f, 300.0f, 100.0f));
}

void AHRBGameMode::InitGameState()
{
	Super::InitGameState();

	// GameState에 ExperienceManagerComponent 부착
	AGameStateBase* GS = GameState;
	check(GS);

	UHRBExperienceManagerComponent* ExperienceComponent = NewObject<UHRBExperienceManagerComponent>(GS);
	check(ExperienceComponent);
	ExperienceComponent->RegisterComponent();

	UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] ExperienceManagerComponent를 GameState에 등록 완료"));

	// InitGameState()에서 Experience 설정 — InitGame()보다 나중에 호출되므로 컴포넌트가 보장됨
	if (!DefaultExperience.IsNull())
	{
		// Experience 로드 완료 콜백 등록
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(
			FOnHRBExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));

		// Experience 설정 (동기 로딩)
		ExperienceComponent->SetCurrentExperience(DefaultExperience);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HRBGameMode] DefaultExperience가 설정되지 않음. 에디터에서 설정 필요."));
	}
}

void AHRBGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	// Experience 로딩은 InitGameState()에서 처리 (컴포넌트 등록 순서 보장)
}

void AHRBGameMode::OnExperienceLoaded(const UHRBExperienceDefinition* CurrentExperience)
{
	UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] Experience 로드 완료"));

	// 대기 중인 플레이어들 스폰 재시도 (팀 배정은 HandleStartingNewPlayer에서 처리)
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Iterator->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
	// 이미 접속한 PC들은 HandleStartingNewPlayer로 스폰 처리됨
}

void AHRBGameMode::SpawnTeamForController(APlayerController* PC,
	TSubclassOf<AHRBHeroCharacter> TeamClass, const TArray<FVector>& SpawnLocs)
{
	UWorld* World = GetWorld();
	if (!World || !TeamClass || !PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HRBGameMode] SpawnTeamForController 실패: World/TeamClass/PC null"));
		return;
	}

	AHRBPlayerController* HRBPC = Cast<AHRBPlayerController>(PC);

	for (int32 i = 0; i < 3; ++i)
	{
		const FVector SpawnLocation = SpawnLocs.IsValidIndex(i)
			? SpawnLocs[i]
			: FVector(i * 200.0f, 0.0f, 100.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PC; // Server RPC 라우팅을 위해 Owner 설정
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AHRBHeroCharacter* Hero = World->SpawnActor<AHRBHeroCharacter>(
			TeamClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams);

		if (Hero)
		{
			Hero->HeroIndex = (NextTeamIndex * 10) + i; // Team 0: 0,1,2 / Team 1: 10,11,12

			if (TeamClass == EnemyHeroCharacterClass)
			{
				SpawnedEnemies.Add(Cast<AHRBEnemyHeroCharacter>(Hero));
			}
			else
			{
				SpawnedHeroes.Add(Hero);
			}

			if (HRBPC)
			{
				HRBPC->RegisterHero(Hero);
			}

			UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] Spawned %s (HeroIndex=%d) at %s"),
				*GetNameSafe(Hero), Hero->HeroIndex, *SpawnLocation.ToString());
		}
	}

	// ── 카메라 폰을 팀 spawn 평균 위치로 이동 (시점 분리) ──
	if (APawn* CameraPawn = PC->GetPawn())
	{
		FVector AvgLoc = FVector::ZeroVector;
		int32 Count = 0;
		for (const FVector& L : SpawnLocs)
		{
			AvgLoc += L;
			Count++;
		}
		if (Count > 0)
		{
			AvgLoc /= Count;
		}

		// 팀 구분: HeroCharacterClass 면 Hero 팀(+X 향함), 아니면 Enemy 팀(-X 향함)
		const bool bIsHeroTeam = (TeamClass == HeroCharacterClass);
		const FRotator FaceRot = bIsHeroTeam
			? FRotator(0.f, 0.f, 0.f)        // Hero 팀: +X 방향(상대 쪽)
			: FRotator(0.f, 180.f, 0.f);     // Enemy 팀: -X 방향(상대 쪽)

		CameraPawn->SetActorLocationAndRotation(
			AvgLoc + FVector(0.f, 0.f, 50.f),
			FaceRot,
			/*bSweep=*/ false,
			/*OutSweepHitResult=*/ nullptr,
			ETeleportType::TeleportPhysics);

		UE_LOG(LogTemp, Log,
			TEXT("[HRBGameMode] Camera Pawn moved to team center: %s @ %s (Team=%s)"),
			*CameraPawn->GetName(), *AvgLoc.ToString(),
			bIsHeroTeam ? TEXT("Hero") : TEXT("Enemy"));
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[HRBGameMode] SpawnTeamForController: PC has no Pawn — camera move skipped"));
	}
}

bool AHRBGameMode::IsExperienceLoaded() const
{
	AGameStateBase* GS = GameState;
	if (!GS)
	{
		return false;
	}

	UHRBExperienceManagerComponent* ExperienceComponent = GS->FindComponentByClass<UHRBExperienceManagerComponent>();
	if (!ExperienceComponent)
	{
		return false;
	}

	return ExperienceComponent->IsExperienceLoaded();
}

const UHRBPawnData* AHRBGameMode::GetPawnDataForController(const AController* InController) const
{
	// Experience에서 PawnData를 가져옴
	AGameStateBase* GS = GameState;
	if (!GS)
	{
		return nullptr;
	}

	UHRBExperienceManagerComponent* ExperienceComponent = GS->FindComponentByClass<UHRBExperienceManagerComponent>();
	if (!ExperienceComponent || !ExperienceComponent->IsExperienceLoaded())
	{
		return nullptr;
	}

	const UHRBExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
	if (Experience && Experience->DefaultPawnData)
	{
		return Experience->DefaultPawnData;
	}

	return nullptr;
}

UClass* AHRBGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	const UHRBPawnData* PawnData = GetPawnDataForController(InController);
	if (PawnData && PawnData->PawnClass)
	{
		return PawnData->PawnClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AHRBGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Experience가 아직 로드되지 않았으면 스폰을 지연
	if (!IsExperienceLoaded())
	{
		UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] Experience 미로드 - 플레이어 스폰 대기: %s"), *GetNameSafe(NewPlayer));
		return;
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// 접속 순서로 팀 배정 — 첫 PC: Team 0 (Hero), 두 번째 PC: Team 1 (Enemy)
	if (NextTeamIndex == 0)
	{
		SpawnTeamForController(NewPlayer, HeroCharacterClass, HeroSpawnLocations);
		UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] %s -> Team 0 (Hero)"), *GetNameSafe(NewPlayer));
	}
	else
	{
		SpawnTeamForController(NewPlayer,
			TSubclassOf<AHRBHeroCharacter>(EnemyHeroCharacterClass),
			EnemySpawnLocations);
		UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] %s -> Team 1 (Enemy)"), *GetNameSafe(NewPlayer));
	}

	NextTeamIndex++;
}

bool AHRBGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	if (!IsExperienceLoaded())
	{
		return false;
	}

	return Super::PlayerCanRestart_Implementation(Player);
}

void AHRBGameMode::CheckRoundEnd()
{
	// 플레이어 팀 생존 확인
	bool bPlayerTeamAlive = false;
	for (const TObjectPtr<AHRBHeroCharacter>& Hero : SpawnedHeroes)
	{
		if (Hero && !Hero->bIsDead)
		{
			bPlayerTeamAlive = true;
			break;
		}
	}

	// 적 팀 생존 확인
	bool bEnemyTeamAlive = false;
	for (const TObjectPtr<AHRBEnemyHeroCharacter>& Enemy : SpawnedEnemies)
	{
		if (Enemy && !Enemy->bIsDead)
		{
			bEnemyTeamAlive = true;
			break;
		}
	}

	if (!bPlayerTeamAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] 라운드 종료 - 적 팀 승리"));
	}
	else if (!bEnemyTeamAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] 라운드 종료 - 플레이어 팀 승리"));
	}
}
