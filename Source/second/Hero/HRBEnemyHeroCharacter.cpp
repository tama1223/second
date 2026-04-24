// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBEnemyHeroCharacter.h"

#include "AI/HRBEnemyAIController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBEnemyHeroCharacter)

AHRBEnemyHeroCharacter::AHRBEnemyHeroCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AHRBEnemyAIController::StaticClass();

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

	// 빨간색 머티리얼로 변경하여 적 구분.
	// NormalMaterial도 빨간색으로 덮어써야 ResetHitReaction() 이후에도 빨간색이 유지된다.
	if (BodyMesh)
	{
		UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (BaseMat)
		{
			UMaterialInstanceDynamic* RedMat = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (RedMat)
			{
				RedMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.1f, 0.1f, 1.0f));
				BodyMesh->SetMaterial(0, RedMat);
				// 피격 반응 후 복귀 머티리얼도 빨간색으로 갱신
				NormalMaterial = RedMat;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBEnemyHeroCharacter] Enemy Hero spawned"));
}
