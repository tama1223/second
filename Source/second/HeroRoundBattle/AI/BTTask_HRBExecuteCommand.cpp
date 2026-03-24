#include "HeroRoundBattle/AI/BTTask_HRBExecuteCommand.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HeroRoundBattle/AI/HRBAIController.h"
#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"

UBTTask_HRBExecuteCommand::UBTTask_HRBExecuteCommand()
{
	NodeName = "Execute Attack Command";
}

EBTNodeResult::Type UBTTask_HRBExecuteCommand::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AHRBAIController* AIC = Cast<AHRBAIController>(OwnerComp.GetAIOwner());

	if (!BB || !AIC) return EBTNodeResult::Failed;

	AHRBHeroCharacter* MyHero = AIC->GetControlledHero();
	if (!MyHero || MyHero->IsDead()) return EBTNodeResult::Failed;

	AHRBHeroCharacter* Target = Cast<AHRBHeroCharacter>(
		BB->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (!Target || Target->IsDead()) return EBTNodeResult::Failed;

	MyHero->CommandAttack(Target);
	return EBTNodeResult::Succeeded;
}
