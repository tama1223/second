// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBEnemyHeroCharacter.h"

#include "AI/HRBEnemyAIController.h"
#include "Components/StaticMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBEnemyHeroCharacter)

AHRBEnemyHeroCharacter::AHRBEnemyHeroCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AHRBEnemyAIController::StaticClass();
}

void AHRBEnemyHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 빨간색 머티리얼로 변경하여 적 구분
	UStaticMeshComponent* Body = FindComponentByClass<UStaticMeshComponent>();
	if (Body)
	{
		UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (BaseMat)
		{
			UMaterialInstanceDynamic* RedMat = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (RedMat)
			{
				RedMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.1f, 0.1f, 1.0f));
				Body->SetMaterial(0, RedMat);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBEnemyHeroCharacter] Enemy Hero spawned"));
}
