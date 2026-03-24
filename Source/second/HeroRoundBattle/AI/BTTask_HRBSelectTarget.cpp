#include "HeroRoundBattle/AI/BTTask_HRBSelectTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HeroRoundBattle/AI/HRBAIController.h"
#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"

UBTTask_HRBSelectTarget::UBTTask_HRBSelectTarget()
{
	NodeName = "Select Best Target";
}

EBTNodeResult::Type UBTTask_HRBSelectTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AHRBAIController* AIC = Cast<AHRBAIController>(OwnerComp.GetAIOwner());

	if (!BB || !AIC) return EBTNodeResult::Failed;

	AHRBHeroCharacter* Target = AIC->FindBestTarget();
	if (Target)
	{
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, Target);
		return EBTNodeResult::Succeeded;
	}

	BB->ClearValue(TargetActorKey.SelectedKeyName);
	return EBTNodeResult::Failed;
}
