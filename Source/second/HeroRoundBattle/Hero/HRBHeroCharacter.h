#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeroRoundBattle/HRBTypes.h"
#include "HRBHeroCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeroHPChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeroDied, AHRBHeroCharacter*, Hero);

UCLASS()
class SECOND_API AHRBHeroCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHRBHeroCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- Setup ---
	void InitHero(const FHeroDefinition& InDef, int32 InOwningPlayer, int32 InHeroIndex);
	void ApplyLevelUp(int32 NewLevel);
	void ResetForNewRound();

	// --- Commands (called from PlayerController via ServerRPC) ---
	void CommandMoveTo(const FVector& Destination);
	void CommandAttack(AHRBHeroCharacter* Target);
	void CommandStop();

	// --- Combat ---
	void ReceiveDamage(float RawDamage, AHRBHeroCharacter* Attacker);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitEffect();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDeathEffect();

	// --- Getters ---
	FORCEINLINE int32 GetOwningPlayer() const { return OwningPlayerIndex; }
	FORCEINLINE int32 GetHeroIndex() const { return HeroIndex; }
	FORCEINLINE bool IsDead() const { return bIsDead; }
	FORCEINLINE float GetCurrentHP() const { return CurrentHP; }
	FORCEINLINE float GetMaxHP() const { return EffectiveStats.MaxHP; }
	FORCEINLINE const FHeroDefinition& GetHeroDef() const { return HeroDefinition; }
	FORCEINLINE int32 GetHeroLevel() const { return HeroLevel; }

	UPROPERTY(BlueprintAssignable)
	FOnHeroHPChanged OnHPChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHeroDied OnDied;

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	int32 OwningPlayerIndex = -1;

	UPROPERTY(Replicated, VisibleAnywhere)
	int32 HeroIndex = 0;

	UPROPERTY(Replicated, VisibleAnywhere)
	int32 HeroLevel = 1;

	UPROPERTY(Replicated, VisibleAnywhere)
	FHeroDefinition HeroDefinition;

	UPROPERTY(Replicated, VisibleAnywhere)
	FHeroStats EffectiveStats;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentHP, VisibleAnywhere)
	float CurrentHP = 100.f;

	UPROPERTY(Replicated, VisibleAnywhere)
	bool bIsDead = false;

	UFUNCTION()
	void OnRep_CurrentHP();

	// --- Internal combat state (server only) ---
	UPROPERTY()
	TWeakObjectPtr<AHRBHeroCharacter> AttackTarget;

	FVector MoveDestination;
	bool bHasMoveCommand = false;
	float AttackCooldownRemaining = 0.f;

	void TickCombat(float DeltaTime);
	void PerformAttack();
	void Die();

	// --- Visuals ---
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> HeroMesh;

	UPROPERTY(Replicated)
	FLinearColor HeroColor;

	void UpdateVisuals();
};
