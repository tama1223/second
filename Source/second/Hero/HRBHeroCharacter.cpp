// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBHeroCharacter.h"

#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBHeroCharacter)

AHRBHeroCharacter::AHRBHeroCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Possess 하지 않음
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::Disabled;

	// 캡슐 기본 크기
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));

	// 시각적 표현용 실린더 메시
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetRootComponent());
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	BodyMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 1.5f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 콜리전은 캡슐이 담당

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(
		TEXT("/Engine/BasicShapes/Cylinder"));
	if (CylinderFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CylinderFinder.Object);
	}

	// 선택 데칼 생성
	SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal"));
	SelectionDecal->SetupAttachment(GetRootComponent());
	SelectionDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f)); // 바닥을 향하도록
	SelectionDecal->DecalSize = FVector(64.0f, 64.0f, 64.0f);
	SelectionDecal->SetVisibility(false); // 기본 비선택 상태

	// 이동 비활성화 (AI가 직접 제어할 때까지)
	GetCharacterMovement()->GravityScale = 1.0f;
}

void AHRBHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 데칼 머티리얼을 동적으로 생성 (간단한 색상 데칼)
	if (SelectionDecal)
	{
		UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
			TEXT("/Engine/EngineMaterials/DefaultDeferredDecalMaterial"));
		if (BaseMat)
		{
			UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (DynMat)
			{
				DynMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
				SelectionDecal->SetDecalMaterial(DynMat);
			}
		}
		SelectionDecal->SetVisibility(false);
	}
}

void AHRBHeroCharacter::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;

	if (SelectionDecal)
	{
		SelectionDecal->SetVisibility(bSelected);
	}

	// 메시 아웃라인 효과 (커스텀 뎁스 기반)
	if (BodyMesh)
	{
		BodyMesh->SetRenderCustomDepth(bSelected);
		BodyMesh->SetCustomDepthStencilValue(bSelected ? 1 : 0);
	}
	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(bSelected);
		GetMesh()->SetCustomDepthStencilValue(bSelected ? 1 : 0);
	}
}
