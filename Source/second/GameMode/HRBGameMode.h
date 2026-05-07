// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "HRBGameMode.generated.h"

class UHRBExperienceDefinition;
class UHRBExperienceManagerComponent;
class UHRBPawnData;
class AHRBHeroCharacter;
class AHRBEnemyHeroCharacter;

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

	/** 특정 PlayerController에 팀에 맞는 3체 스폰 + PC에 등록 */
	void SpawnTeamForController(APlayerController* PC, TSubclassOf<AHRBHeroCharacter> TeamClass, const TArray<FVector>& SpawnLocs);

public:
	/** 라운드 종료 체크 (영웅 사망 시 호출) */
	void CheckRoundEnd();

protected:
	/** 인터미션 종료 후 전체 영웅 리스폰 */
	void RestartRound();

	/** 라운드 인터미션 타이머 핸들 */
	FTimerHandle RoundIntermissionTimer;

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

	/** 적 영웅 캐릭터 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "HRB|Hero")
	TSubclassOf<AHRBEnemyHeroCharacter> EnemyHeroCharacterClass;

	/** 적 영웅 스폰 위치 (3개) */
	UPROPERTY(EditDefaultsOnly, Category = "HRB|Hero")
	TArray<FVector> EnemySpawnLocations;

	/** 스폰된 영웅 목록 (PC 등록용 캐시) */
	UPROPERTY()
	TArray<TObjectPtr<AHRBHeroCharacter>> SpawnedHeroes;

	/** 스폰된 적 영웅 목록 */
	UPROPERTY()
	TArray<TObjectPtr<AHRBEnemyHeroCharacter>> SpawnedEnemies;

	/** 접속 순서로 팀 번호 할당 (0 = Hero 팀, 1 = Enemy 팀) */
	UPROPERTY()
	int32 NextTeamIndex = 0;
};
