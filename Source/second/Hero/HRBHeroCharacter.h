// Copyright HeroRoundBattle Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HRBHeroCharacter.generated.h"

class UDecalComponent;
class UTextRenderComponent;
class AAIController;
class UHRBHealthBarComponent;
class AHRBGameMode;
class AHRBEnemyHeroCharacter;
class UAnimSequence;

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

	virtual void Tick(float DeltaTime) override;

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

	// ---------- 공격이동 (A+클릭) ----------

	/** 공격이동 감지 범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HRB|Combat")
	float AttackMoveDetectionRange = 600.0f;

	/** 공격이동 목적지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|Combat")
	FVector AttackMoveDestination;

	/** 공격이동 중 교전 대상 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|Combat")
	TObjectPtr<AHRBHeroCharacter> AttackMoveTarget;

	/** 공격이동 시작: 이동 + 적 탐색 타이머 */
	UFUNCTION(BlueprintCallable, Category = "HRB|Combat")
	void AttackMoveToLocation(const FVector& Destination);

	/** 공격이동 정지 */
	UFUNCTION(BlueprintCallable, Category = "HRB|Combat")
	void StopAttackMove();

	// ---------- 전투 시각 피드백 ----------

	/** 타겟 데칼 표시/숨김 */
	UFUNCTION(BlueprintCallable, Category = "HRB|Combat")
	void SetTargeted(bool bInTargeted);

	/** 공격 시 시각 이펙트 (피격자 위치에 히트 스피어) */
	void PlayAttackEffect(AActor* Target);

protected:
	virtual void BeginPlay() override;

	/** HP바 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HRB|UI")
	TObjectPtr<UHRBHealthBarComponent> HealthBarComp;

	/** 마지막 공격 시간 (쿨다운 체크용) */
	float LastAttackTime = -999.0f;

	/** 공격이동 스캔 타이머 */
	FTimerHandle AttackMoveScanTimer;

	/** 공격이동 스캔 간격 (초) */
	float AttackMoveScanInterval = 0.3f;

	/** 공격이동 활성 여부 */
	bool bIsAttackMoving = false;

	/** 공격이동 타이머 콜백 */
	void AttackMoveScanTick();

	/** 감지 범위 내 가장 가까운 적 영웅 탐색 */
	AHRBEnemyHeroCharacter* FindEnemyInDetectionRange();

	/** Walking 애니 (기본 재생, 항상 루프) */
	UPROPERTY()
	TObjectPtr<UAnimSequence> WalkAnim;

	/** Attack 애니 (공격 시 1회 재생 후 Walking 복귀) */
	UPROPERTY()
	TObjectPtr<UAnimSequence> AttackAnim;

	/** Attack 애니 종료 → Walking 복귀 타이머 */
	FTimerHandle AttackAnimReturnTimer;

	/** Walking 애니로 복귀 */
	void ReturnToWalkingAnim();

private:
	/** 선택 여부 */
	bool bSelected = false;

	/** 선택 하이라이트 데칼 */
	UPROPERTY(VisibleAnywhere, Category = "HRB|Hero")
	TObjectPtr<UDecalComponent> SelectionDecal;

	/** 타겟 표시 데칼 (빨간 원) */
	UPROPERTY(VisibleAnywhere, Category = "HRB|Combat")
	TObjectPtr<UDecalComponent> TargetDecal;
};
