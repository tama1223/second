// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Player/HRBPlayerController.h"

#include "Hero/HRBHeroCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HRBPlayerController)

AHRBPlayerController::AHRBPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AHRBPlayerController::RegisterHero(AHRBHeroCharacter* Hero)
{
	if (Hero && !Heroes.Contains(Hero))
	{
		Heroes.Add(Hero);
		UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] Hero registered: Index=%d"), Hero->HeroIndex);
	}
}

void AHRBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 마우스 커서 표시 + 게임 입력 모두 활성화
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AHRBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// ---- InputAction 생성 ----
	IA_Select = NewObject<UInputAction>(this, TEXT("IA_Select"));
	IA_Select->ValueType = EInputActionValueType::Boolean;

	IA_SelectHero1 = NewObject<UInputAction>(this, TEXT("IA_SelectHero1"));
	IA_SelectHero1->ValueType = EInputActionValueType::Boolean;

	IA_SelectHero2 = NewObject<UInputAction>(this, TEXT("IA_SelectHero2"));
	IA_SelectHero2->ValueType = EInputActionValueType::Boolean;

	IA_SelectHero3 = NewObject<UInputAction>(this, TEXT("IA_SelectHero3"));
	IA_SelectHero3->ValueType = EInputActionValueType::Boolean;

	// ---- MappingContext 생성 ----
	IMC_Selection = NewObject<UInputMappingContext>(this, TEXT("IMC_Selection"));

	IMC_Selection->MapKey(IA_Select, EKeys::LeftMouseButton);
	IMC_Selection->MapKey(IA_SelectHero1, EKeys::One);
	IMC_Selection->MapKey(IA_SelectHero2, EKeys::Two);
	IMC_Selection->MapKey(IA_SelectHero3, EKeys::Three);

	// ---- 매핑 컨텍스트 등록 ----
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(IMC_Selection, 1); // Priority 1 (아레나 카메라보다 높음)
	}

	// ---- 액션 바인딩 ----
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(IA_Select, ETriggerEvent::Started, this, &ThisClass::HandleSelectStarted);
		EIC->BindAction(IA_Select, ETriggerEvent::Completed, this, &ThisClass::HandleSelectCompleted);
		EIC->BindAction(IA_SelectHero1, ETriggerEvent::Started, this, &ThisClass::HandleSelectHero1);
		EIC->BindAction(IA_SelectHero2, ETriggerEvent::Started, this, &ThisClass::HandleSelectHero2);
		EIC->BindAction(IA_SelectHero3, ETriggerEvent::Started, this, &ThisClass::HandleSelectHero3);
	}
}

void AHRBPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// 마우스 좌클릭이 눌려있는 동안 현재 마우스 위치 갱신 (드래그 추적)
	// bIsDragging은 DrawHUD에서 threshold 이상일 때 설정되므로,
	// 위치 갱신은 버튼이 눌린 상태(DragStartScreen이 초기화된 이후)라면 항상 수행
	if (IsInputKeyDown(EKeys::LeftMouseButton))
	{
		float MouseX, MouseY;
		if (GetMousePosition(MouseX, MouseY))
		{
			DragCurrentScreen = FVector2D(MouseX, MouseY);
		}
	}
}

void AHRBPlayerController::HandleSelectStarted(const FInputActionValue& Value)
{
	// 드래그 시작 위치 기록
	float MouseX, MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		DragStartScreen = FVector2D(MouseX, MouseY);
		DragCurrentScreen = DragStartScreen;
		bIsDragging = false; // 아직 드래그 아님, 클릭 시작만
	}
}

void AHRBPlayerController::HandleSelectCompleted(const FInputActionValue& Value)
{
	float MouseX, MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		DragCurrentScreen = FVector2D(MouseX, MouseY);
	}

	// 드래그 거리 확인
	const float DragDistance = FVector2D::Distance(DragStartScreen, DragCurrentScreen);

	if (DragDistance > DragThreshold)
	{
		// 드래그 범위 선택
		SelectHeroesInDragRect();
	}
	else
	{
		// 단일 클릭 선택
		SelectHeroUnderCursor();
	}

	bIsDragging = false;
}

void AHRBPlayerController::SelectHeroUnderCursor()
{
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Pawn, false, HitResult))
	{
		if (AHRBHeroCharacter* HitHero = Cast<AHRBHeroCharacter>(HitResult.GetActor()))
		{
			ClearSelection();
			HitHero->SetSelected(true);
			SelectedHeroes.Add(HitHero);
			UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] Hero selected: Index=%d"), HitHero->HeroIndex);
			return;
		}
	}

	// 빈 곳 클릭 시 선택 해제
	ClearSelection();
}

void AHRBPlayerController::SelectHeroByIndex(int32 Index)
{
	if (Heroes.IsValidIndex(Index))
	{
		ClearSelection();
		AHRBHeroCharacter* Hero = Heroes[Index];
		if (Hero)
		{
			Hero->SetSelected(true);
			SelectedHeroes.Add(Hero);
			UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] Hero selected by key: Index=%d"), Index);
		}
	}
}

void AHRBPlayerController::ClearSelection()
{
	for (AHRBHeroCharacter* Hero : SelectedHeroes)
	{
		if (Hero)
		{
			Hero->SetSelected(false);
		}
	}
	SelectedHeroes.Empty();
}

void AHRBPlayerController::SelectHeroesInDragRect()
{
	ClearSelection();

	// 드래그 사각형 영역 계산
	const float MinX = FMath::Min(DragStartScreen.X, DragCurrentScreen.X);
	const float MaxX = FMath::Max(DragStartScreen.X, DragCurrentScreen.X);
	const float MinY = FMath::Min(DragStartScreen.Y, DragCurrentScreen.Y);
	const float MaxY = FMath::Max(DragStartScreen.Y, DragCurrentScreen.Y);

	for (AHRBHeroCharacter* Hero : Heroes)
	{
		if (!Hero)
		{
			continue;
		}

		// 영웅의 월드 위치를 스크린 좌표로 변환
		FVector2D ScreenPos;
		if (ProjectWorldLocationToScreen(Hero->GetActorLocation(), ScreenPos))
		{
			if (ScreenPos.X >= MinX && ScreenPos.X <= MaxX &&
				ScreenPos.Y >= MinY && ScreenPos.Y <= MaxY)
			{
				Hero->SetSelected(true);
				SelectedHeroes.Add(Hero);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] Drag select: %d heroes selected"), SelectedHeroes.Num());
}

void AHRBPlayerController::HandleSelectHero1(const FInputActionValue& Value)
{
	SelectHeroByIndex(0);
}

void AHRBPlayerController::HandleSelectHero2(const FInputActionValue& Value)
{
	SelectHeroByIndex(1);
}

void AHRBPlayerController::HandleSelectHero3(const FInputActionValue& Value)
{
	SelectHeroByIndex(2);
}
