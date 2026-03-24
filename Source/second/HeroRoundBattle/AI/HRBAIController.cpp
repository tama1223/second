#include "HeroRoundBattle/AI/HRBAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"
#include "HeroRoundBattle/Core/HRBGameState.h"
#include "EngineUtils.h"

AHRBAIController::AHRBAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHRBAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Try to load BT asset at runtime
	if (!HeroBehaviorTree)
	{
		HeroBehaviorTree = LoadObject<UBehaviorTree>(
			nullptr, TEXT("/Game/HeroRoundBattle/AI/BT_HRBHero.BT_HRBHero"));
	}

	// Also try to load BB asset and link it to BT (Python can't set protected property)
	if (HeroBehaviorTree && !HeroBehaviorTree->BlackboardAsset)
	{
		UBlackboardData* BBAsset = LoadObject<UBlackboardData>(
			nullptr, TEXT("/Game/HeroRoundBattle/AI/BB_HRBHero.BB_HRBHero"));
		if (BBAsset)
		{
			HeroBehaviorTree->BlackboardAsset = BBAsset;
		}
	}

	if (HeroBehaviorTree && HeroBehaviorTree->BlackboardAsset)
	{
		UBlackboardComponent* BBComp = nullptr;
		UseBlackboard(HeroBehaviorTree->BlackboardAsset, BBComp);
		RunBehaviorTree(HeroBehaviorTree);
		bUseFallbackAI = false;
		UE_LOG(LogTemp, Log, TEXT("HRBAIController: Running BehaviorTree for %s"), *InPawn->GetName());
	}
	else
	{
		bUseFallbackAI = true;
		UE_LOG(LogTemp, Warning, TEXT("HRBAIController: BT not fully configured, using fallback AI for %s"), *InPawn->GetName());
	}
}

void AHRBAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bUseFallbackAI)
	{
		TickFallbackAI(DeltaTime);
	}
}

AHRBHeroCharacter* AHRBAIController::GetControlledHero() const
{
	return Cast<AHRBHeroCharacter>(GetPawn());
}

TArray<AHRBHeroCharacter*> AHRBAIController::GetEnemyHeroes() const
{
	TArray<AHRBHeroCharacter*> Enemies;
	AHRBHeroCharacter* MyHero = GetControlledHero();
	if (!MyHero) return Enemies;

	const int32 MyTeam = MyHero->GetOwningPlayer();

	for (TActorIterator<AHRBHeroCharacter> It(GetWorld()); It; ++It)
	{
		AHRBHeroCharacter* Other = *It;
		if (Other && !Other->IsDead() && Other->GetOwningPlayer() != MyTeam)
		{
			Enemies.Add(Other);
		}
	}
	return Enemies;
}

AHRBHeroCharacter* AHRBAIController::FindBestTarget() const
{
	TArray<AHRBHeroCharacter*> Enemies = GetEnemyHeroes();
	if (Enemies.Num() == 0) return nullptr;

	AHRBHeroCharacter* MyHero = GetControlledHero();
	if (!MyHero) return nullptr;

	// Strategy: focus fire on the enemy with lowest HP percentage
	AHRBHeroCharacter* BestTarget = nullptr;
	float LowestHPPercent = TNumericLimits<float>::Max();

	for (AHRBHeroCharacter* Enemy : Enemies)
	{
		float HPPercent = (Enemy->GetMaxHP() > 0.f)
			? (Enemy->GetCurrentHP() / Enemy->GetMaxHP())
			: 0.f;

		if (HPPercent < LowestHPPercent)
		{
			LowestHPPercent = HPPercent;
			BestTarget = Enemy;
		}
	}

	return BestTarget;
}

void AHRBAIController::TickFallbackAI(float DeltaTime)
{
	AHRBHeroCharacter* MyHero = GetControlledHero();
	if (!MyHero || MyHero->IsDead()) return;

	// Only act during active round
	AHRBGameState* GS = GetWorld()->GetGameState<AHRBGameState>();
	if (!GS || GS->RoundPhase != ERoundPhase::RoundActive) return;

	FallbackDecisionTimer -= DeltaTime;
	if (FallbackDecisionTimer > 0.f) return;
	FallbackDecisionTimer = FallbackDecisionInterval;

	// Focus fire on weakest enemy
	AHRBHeroCharacter* Target = FindBestTarget();
	if (Target)
	{
		MyHero->CommandAttack(Target);
	}
}
