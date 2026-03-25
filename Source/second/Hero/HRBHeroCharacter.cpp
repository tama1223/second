// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Hero/HRBHeroCharacter.h"

#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBHeroCharacter)

AHRBHeroCharacter::AHRBHeroCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// PlayerController로 Possess 하지 않음
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	// AIController는 자동 Possess (이동 명령에 필요)
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

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

void AHRBHeroCharacter::MoveToLocation(const FVector& Destination)
{
	MoveDestination = Destination;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->MoveToLocation(Destination, 50.0f, /*bStopOnOverlap=*/true,
			/*bUsePathfinding=*/true, /*bProjectDestinationToNavigation=*/true);
		UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: MoveToLocation (%.0f, %.0f, %.0f)"),
			HeroIndex, Destination.X, Destination.Y, Destination.Z);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HRBHeroCharacter] Hero %d: No AIController, cannot move"), HeroIndex);
	}
}

void AHRBHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	// AIController가 없으면 스폰 (이동 명령에 필요)
	if (!GetController())
	{
		SpawnDefaultController();
	}

	// BasicShapeMaterial로 BodyMesh 기본 머티리얼 설정
	if (BodyMesh)
	{
		UMaterial* BaseMat = LoadObject<UMaterial>(nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
		if (BaseMat)
		{
			NormalMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			SelectedMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
			if (NormalMaterial)
			{
				NormalMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.8f, 0.8f, 0.8f, 1.0f)); // 회색
				BodyMesh->SetMaterial(0, NormalMaterial);
			}
			if (SelectedMaterial)
			{
				SelectedMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.0f, 1.0f, 0.2f, 1.0f)); // 초록
			}
		}
	}

	// 데칼은 사용하지 않음
	if (SelectionDecal)
	{
		SelectionDecal->SetVisibility(false);
	}
}

void AHRBHeroCharacter::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;

	// BodyMesh 머티리얼로 선택 상태 표시 (회색 ↔ 초록)
	if (BodyMesh)
	{
		if (bSelected && SelectedMaterial)
		{
			BodyMesh->SetMaterial(0, SelectedMaterial);
		}
		else if (!bSelected && NormalMaterial)
		{
			BodyMesh->SetMaterial(0, NormalMaterial);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBHeroCharacter] Hero %d: %s"),
		HeroIndex, bSelected ? TEXT("Selected (green)") : TEXT("Deselected (grey)"));
}
