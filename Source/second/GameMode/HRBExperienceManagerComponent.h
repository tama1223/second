// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HRBExperienceManagerComponent.generated.h"

class UHRBExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHRBExperienceLoaded, const UHRBExperienceDefinition* /*Experience*/);

/**
 * Experience 로드 상태
 */
enum class EHRBExperienceLoadState : uint8
{
	Unloaded,
	Loading,
	Loaded
};

/**
 * UHRBExperienceManagerComponent
 *
 * Lyra ULyraExperienceManagerComponent 패턴.
 * GameState에 부착되어 Experience의 로딩/관리를 담당한다.
 * Step 1에서는 동기 로딩으로 간소화.
 */
UCLASS()
class SECOND_API UHRBExperienceManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHRBExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Experience를 설정하고 로드한다. (동기 로딩) */
	void SetCurrentExperience(TSoftObjectPtr<UHRBExperienceDefinition> ExperienceAsset);

	/** Experience 로드 완료 여부 */
	bool IsExperienceLoaded() const;

	/** 현재 Experience를 반환 (로드 완료 상태에서만 유효, 아니면 check 실패) */
	const UHRBExperienceDefinition* GetCurrentExperienceChecked() const;

	/**
	 * Experience 로드 완료 시 호출되는 델리게이트 등록.
	 * 이미 로드 완료 상태면 즉시 호출.
	 */
	void CallOrRegister_OnExperienceLoaded(FOnHRBExperienceLoaded::FDelegate&& Delegate);

	/** Experience 로드 완료 델리게이트 */
	FOnHRBExperienceLoaded OnExperienceLoaded;

private:
	/** 현재 Experience */
	UPROPERTY()
	TObjectPtr<const UHRBExperienceDefinition> CurrentExperience;

	/** 로드 상태 */
	EHRBExperienceLoadState LoadState = EHRBExperienceLoadState::Unloaded;
};
