// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "GameMode/HRBExperienceManagerComponent.h"

#include "GameMode/HRBExperienceDefinition.h"
#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBExperienceManagerComponent)

UHRBExperienceManagerComponent::UHRBExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentExperience = nullptr;
	LoadState = EHRBExperienceLoadState::Unloaded;
}

void UHRBExperienceManagerComponent::SetCurrentExperience(TSoftObjectPtr<UHRBExperienceDefinition> ExperienceAsset)
{
	check(LoadState == EHRBExperienceLoadState::Unloaded);
	check(!CurrentExperience);

	LoadState = EHRBExperienceLoadState::Loading;

	// Step 1: 동기 로딩
	const UHRBExperienceDefinition* LoadedExperience = ExperienceAsset.LoadSynchronous();

	if (!LoadedExperience)
	{
		UE_LOG(LogTemp, Error, TEXT("[HRBExperienceManager] Experience 로드 실패: %s"), *ExperienceAsset.ToString());
		LoadState = EHRBExperienceLoadState::Unloaded;
		return;
	}

	CurrentExperience = LoadedExperience;
	LoadState = EHRBExperienceLoadState::Loaded;

	UE_LOG(LogTemp, Log, TEXT("[HRBExperienceManager] Experience 로드 완료: %s"), *GetNameSafe(CurrentExperience));

	// 델리게이트 브로드캐스트
	OnExperienceLoaded.Broadcast(CurrentExperience);
}

bool UHRBExperienceManagerComponent::IsExperienceLoaded() const
{
	return LoadState == EHRBExperienceLoadState::Loaded;
}

const UHRBExperienceDefinition* UHRBExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EHRBExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

void UHRBExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnHRBExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		// 이미 로드 완료 — 즉시 호출
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		// 아직 미로드 — 등록
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}
