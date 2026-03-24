#include "HeroRoundBattle/AI/BTService_HRBUpdatePerception.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HeroRoundBattle/AI/HRBAIController.h"
#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"

UBTService_HRBUpdatePerception::UBTService_HRBUpdatePerception()
{
	NodeName = "Update Combat Perception";
	Interval = 0.25f;
	RandomDeviation = 0.05f;
}

void UBTService_HRBUpdatePerception::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AHRBAIController* AIC = Cast<AHRBAIController>(OwnerComp.GetAIOwner());
	if (!AIC) return;

	AHRBHeroCharacter* MyHero = AIC->GetControlledHero();
	if (!MyHero || MyHero->IsDead()) return;

	// Update self HP percent
	const float HPPercent = (MyHero->GetMaxHP() > 0.f)
		? (MyHero->GetCurrentHP() / MyHero->GetMaxHP())
		: 0.f;
	BB->SetValueAsFloat(SelfHPPercentKey.SelectedKeyName, HPPercent);

	// Validate current target
	UObject* TargetObj = BB->GetValueAsObject(TargetActorKey.SelectedKeyName);
	AHRBHeroCharacter* Target = Cast<AHRBHeroCharacter>(TargetObj);

	if (Target && !Target->IsDead())
	{
		const float Distance = FVector::Dist(MyHero->GetActorLocation(), Target->GetActorLocation());
		BB->SetValueAsFloat(TargetDistanceKey.SelectedKeyName, Distance);
	}
	else
	{
		BB->ClearValue(TargetActorKey.SelectedKeyName);
		BB->SetValueAsFloat(TargetDistanceKey.SelectedKeyName, 0.f);
	}
}
