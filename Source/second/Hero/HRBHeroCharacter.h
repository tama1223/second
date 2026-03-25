// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HRBHeroCharacter.generated.h"

class UDecalComponent;
class UStaticMeshComponent;
class AAIController;
class UHRBHealthBarComponent;
class AHRBGameMode;

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

	/** 지정된 위치로 이동 명령 */
	UFUNCTION(BlueprintCallable, Category = "HRB|Hero")
	void MoveToLocation(const FVector& Destination);

	/** 현재 이동 목표 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|Hero")
	FVector MoveDestination;

	// ---------- 전투 스탯 (Step 5) ----------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HRB|Combat")
	float MaxHP = 200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|Combat")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HRB|Combat")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HRB|Combat")
	float Defense = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HRB|Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HRB|Combat")
	float AttackCooldown = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|Combat")
	bool bIsDead = false;

	/** TakeDamage override */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	/** 사망 처리 */
	UFUNCTION(BlueprintCallable, Category = "HRB|Combat")
	void Die();

	/** 대상 공격 */
	UFUNCTION(BlueprintCallable, Category = "HRB|Combat")
	void Attack(AHRBHeroCharacter* Target);

protected:
	virtual void BeginPlay() override;

	/** HP바 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|UI")
	TObjectPtr<UHRBHealthBarComponent> HealthBarComp;

	/** 마지막 공격 시간 (쿨다운 체크용) */
	float LastAttackTime = -999.0f;

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
