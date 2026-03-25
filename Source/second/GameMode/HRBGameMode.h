// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "HRBGameMode.generated.h"

class UHRBExperienceDefinition;
class UHRBExperienceManagerComponent;
class UHRBPawnData;
class AHRBHeroCharacter;

/**
 * AHRBGameMode
 *
 * Lyra ALyraGameMode 패턴.
 * Experience 기반으로 게임을 초기화하는 GameMode.
 */
UCLASS(Config = Game)
class SECOND_API AHRBGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHRBGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** PawnData 조회 */
	const UHRBPawnData* GetPawnDataForController(const AController* InController) const;

	//~ AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void InitGameState() override;
	//~ End of AGameModeBase interface

protected:
	/** Experience 로드 완료 콜백 */
	void OnExperienceLoaded(const UHRBExperienceDefinition* CurrentExperience);

	/** Experience 로드 완료 여부 */
	bool IsExperienceLoaded() const;

	/** 아레나에 영웅 3체 스폰 */
	void SpawnHeroes();

protected:
	/** 에디터에서 설정할 기본 Experience */
	UPROPERTY(EditDefaultsOnly, Category = "HRB|Experience")
	TSoftObjectPtr<UHRBExperienceDefinition> DefaultExperience;

	/** 영웅 스폰 위치 (3개) */
	UPROPERTY(EditDefaultsOnly, Category = "HRB|Hero")
	TArray<FVector> HeroSpawnLocations;

	/** 영웅 캐릭터 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "HRB|Hero")
	TSubclassOf<AHRBHeroCharacter> HeroCharacterClass;
};
