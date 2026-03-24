#pragma once

#include "CoreMinimal.h"
#include "HRBTypes.generated.h"

UENUM(BlueprintType)
enum class ERace : uint8
{
	Human,
	Undead,
	Orc,
	NightElf,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ERoundPhase : uint8
{
	WaitingForPlayers,
	RaceSelect,
	RoundCountdown,
	RoundActive,
	RoundEnd,
	GameOver
};

UENUM(BlueprintType)
enum class EHeroAttackType : uint8
{
	Melee,
	Ranged
};

USTRUCT(BlueprintType)
struct FHeroStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Attack = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHeroAttackType AttackType = EHeroAttackType::Melee;

	// Per-level stat growth
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HPPerLevel = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPerLevel = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefensePerLevel = 1.f;
};

USTRUCT(BlueprintType)
struct FHeroDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HeroID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERace Race = ERace::Human;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHeroStats BaseStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor HeroColor = FLinearColor::White;
};
