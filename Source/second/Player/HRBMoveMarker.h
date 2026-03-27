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

	/** 마커 색상 (기본 녹색) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HRB|Marker")
	FLinearColor MarkerColor = FLinearColor(0.0f, 1.0f, 0.2f, 1.0f);

	/** 마커 색상을 변경 (스폰 후 호출 가능) */
	UFUNCTION(BlueprintCallable, Category = "HRB|Marker")
	void SetMarkerColor(const FLinearColor& NewColor);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "HRB|Marker")
	TObjectPtr<UStaticMeshComponent> MarkerMesh;

	/** 동적 머티리얼 인스턴스 (색상 변경용) */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
};
