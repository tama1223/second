// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "HRBExperienceDefinition.generated.h"

class UHRBPawnData;

/**
 * UHRBExperienceDefinition
 *
 * Lyra ULyraExperienceDefinition 패턴.
 * Experience(게임 경험)를 정의하는 데이터 에셋.
 */
UCLASS(BlueprintType, Const)
class SECOND_API UHRBExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UHRBExperienceDefinition();

public:
	/** 기본 PawnData */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TObjectPtr<const UHRBPawnData> DefaultPawnData;

	/** 활성화할 Game Feature 플러그인 목록 (향후 확장용) */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<FString> GameFeaturesToEnable;
};
