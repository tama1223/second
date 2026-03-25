// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HRBHeroCharacter.generated.h"

class UDecalComponent;
class UStaticMeshComponent;

/**
 * AHRBHeroCharacter
 *
 * 아레나에 스폰되는 영웅 캐릭터.
 * PlayerController가 Possess하지 않으며, 명령으로 제어된다.
 * 선택 시 DecalComponent로 하이라이트 표시.
 */
UCLASS()
class SECOND_API AHRBHeroCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHRBHeroCharacter();

	/** 영웅 인덱스 (0, 1, 2) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|Hero")
	int32 HeroIndex;

	/** 선택 상태 설정 */
	UFUNCTION(BlueprintCallable, Category = "HRB|Hero")
	void SetSelected(bool bInSelected);

	/** 선택 상태 조회 */
	UFUNCTION(BlueprintPure, Category = "HRB|Hero")
	bool IsSelected() const { return bSelected; }

protected:
	virtual void BeginPlay() override;

private:
	/** 선택 여부 */
	bool bSelected = false;

	/** 선택 하이라이트 데칼 */
	UPROPERTY(VisibleAnywhere, Category = "HRB|Hero")
	TObjectPtr<UDecalComponent> SelectionDecal;

	/** 시각적 표현용 스태틱 메시 (기본 실린더) */
	UPROPERTY(VisibleAnywhere, Category = "HRB|Hero")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	/** 비선택 머티리얼 (회색) */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> NormalMaterial;

	/** 선택 머티리얼 (초록) */
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SelectedMaterial;
};
