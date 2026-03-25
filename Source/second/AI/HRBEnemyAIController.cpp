// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "AI/HRBEnemyAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBEnemyAIController)

AHRBEnemyAIController::AHRBEnemyAIController()
{
	// BrainComponent (BehaviorTreeComponent)와 BlackboardComponent는
	// AAIController가 RunBehaviorTree 시 자동 생성
}

void AHRBEnemyAIController::SetupBehaviorTree()
{
	// BT와 BB 에셋은 에디터에서 설정
	// 코드에서는 준비만 해놓음
}

void AHRBEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	SetupBehaviorTree();
}

void AHRBEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		UE_LOG(LogTemp, Log, TEXT("[HRBEnemyAIController] Behavior Tree started for %s"), *GetNameSafe(InPawn));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HRBEnemyAIController] No BehaviorTree set. AI will not run BT for %s"), *GetNameSafe(InPawn));
	}
}
