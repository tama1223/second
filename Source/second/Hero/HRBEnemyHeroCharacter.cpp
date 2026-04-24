// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBEnemyHeroCharacter.h"

#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBEnemyHeroCharacter)

AHRBEnemyHeroCharacter::AHRBEnemyHeroCharacter()
{
	// AI 제거 — 사람 PlayerController가 조종.
	// 부모(AHRBHeroCharacter)에서 기본 AAIController를 사용하는 설정을 유지.

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
