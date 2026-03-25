// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "GameMode/HRBGameMode.h"

#include "GameMode/HRBExperienceDefinition.h"
#include "GameMode/HRBExperienceManagerComponent.h"
#include "Character/HRBPawnData.h"
#include "Hero/HRBHeroCharacter.h"
#include "Player/HRBPlayerController.h"
#include "Player/HRBSelectionHUD.h"
#include "GameFramework/GameStateBase.h"
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
	UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] Experience 로드 완료 - 대기 중인 플레이어를 스폰합니다."));

	// 대기 중인 플레이어들 스폰 재시도
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

	// 영웅 3체 스폰
	SpawnHeroes();
}

void AHRBGameMode::SpawnHeroes()
{
	UWorld* World = GetWorld();
	if (!World || !HeroCharacterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HRBGameMode] SpawnHeroes 실패: World 또는 HeroCharacterClass가 null"));
		return;
	}

	// 첫 번째 PlayerController를 찾아서 영웅 등록
	AHRBPlayerController* HRBPC = nullptr;
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		HRBPC = Cast<AHRBPlayerController>(Iterator->Get());
		if (HRBPC)
		{
			break;
		}
	}

	for (int32 i = 0; i < 3; ++i)
	{
		const FVector SpawnLocation = HeroSpawnLocations.IsValidIndex(i)
			? HeroSpawnLocations[i]
			: FVector(0.0f, i * 200.0f, 100.0f);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AHRBHeroCharacter* Hero = World->SpawnActor<AHRBHeroCharacter>(
			HeroCharacterClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams);

		if (Hero)
		{
			Hero->HeroIndex = i;
			UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] Hero %d spawned at %s"), i, *SpawnLocation.ToString());

			// PlayerController에 등록
			if (HRBPC)
			{
				HRBPC->RegisterHero(Hero);
			}
		}
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
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[HRBGameMode] Experience 미로드 - 플레이어 스폰 대기: %s"), *GetNameSafe(NewPlayer));
	}
}

bool AHRBGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	if (!IsExperienceLoaded())
	{
		return false;
	}

	return Super::PlayerCanRestart_Implementation(Player);
}
