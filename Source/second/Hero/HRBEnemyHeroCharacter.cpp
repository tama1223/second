// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBEnemyHeroCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"
#include "AIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBEnemyHeroCharacter)

AHRBEnemyHeroCharacter::AHRBEnemyHeroCharacter()
{
	// MoveToLocation을 위한 AAIController 자동 possess 명시 (BP override 방지)
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	// Enemy 전용 SkeletalMesh Material override (부모 Hero의 Material_2를 덮어씀)
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> EnemyMatFinder(
		TEXT("/Game/HeroRoundBattle/Characaters/Orc/Materials/Material_1.Material_1"));
	if (EnemyMatFinder.Succeeded())
	{
		GetMesh()->SetMaterial(0, EnemyMatFinder.Object);
	}
}

void AHRBEnemyHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[HRBEnemyHeroCharacter] Enemy Hero spawned"));
}
