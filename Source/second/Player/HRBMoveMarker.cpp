// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Player/HRBMoveMarker.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBMoveMarker)

AHRBMoveMarker::AHRBMoveMarker()
{
	PrimaryActorTick.bCanEverTick = false;

	// 납작한 실린더로 바닥 마커 표현
	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
	SetRootComponent(MarkerMesh);
	MarkerMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.05f)); // 넓고 납작하게
	MarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(
		TEXT("/Engine/BasicShapes/Cylinder"));
	if (CylinderFinder.Succeeded())
	{
		MarkerMesh->SetStaticMesh(CylinderFinder.Object);
	}
}

void AHRBMoveMarker::BeginPlay()
{
	Super::BeginPlay();

	// 동적 머티리얼 생성 후 MarkerColor 적용
	UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
		TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	if (BaseMat && MarkerMesh)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), MarkerColor);
			MarkerMesh->SetMaterial(0, DynamicMaterial);
		}
	}

	// 자동 소멸
	SetLifeSpan(LifeSpan);
}

void AHRBMoveMarker::SetMarkerColor(const FLinearColor& NewColor)
{
	MarkerColor = NewColor;
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("Color"), MarkerColor);
	}
}
