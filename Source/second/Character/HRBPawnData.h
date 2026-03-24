// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "HRBPawnData.generated.h"

class UHRBCameraMode;

/**
 * UHRBPawnData
 *
 * Lyra ULyraPawnData 패턴.
 * Pawn 정의에 필요한 불변 데이터를 담는 데이터 에셋.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "HRB Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class SECOND_API UHRBPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UHRBPawnData();

public:
	/** 이 PawnData에 대응되는 Pawn 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HRB|Pawn")
	TSubclassOf<APawn> PawnClass;

	/** 기본 카메라 모드 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HRB|Camera")
	TSubclassOf<UHRBCameraMode> DefaultCameraMode;
};
