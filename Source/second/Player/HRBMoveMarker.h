// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HRBMoveMarker.generated.h"

class UStaticMeshComponent;

/**
 * AHRBMoveMarker
 *
 * 이동 명령 시 목표 지점에 표시되는 시각적 마커.
 * 일정 시간 후 자동 소멸.
 * StaticMesh(실린더) + 동적 머티리얼로 녹색 마커를 표시.
 */
UCLASS()
class SECOND_API AHRBMoveMarker : public AActor
{
	GENERATED_BODY()

public:
	AHRBMoveMarker();

	/** 마커 자동 소멸 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "HRB|Marker")
	float LifeSpan = 0.5f;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "HRB|Marker")
	TObjectPtr<UStaticMeshComponent> MarkerMesh;
};
