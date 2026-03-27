// Copyright HeroRoundBattle Project. All Rights Reserved.

#include "Player/HRBPlayerController.h"

#include "Hero/HRBHeroCharacter.h"
#include "Hero/HRBEnemyHeroCharacter.h"
#include "Player/HRBMoveMarker.h"
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

	IA_MoveCommand = NewObject<UInputAction>(this, TEXT("IA_MoveCommand"));
	IA_MoveCommand->ValueType = EInputActionValueType::Boolean;

	IA_AttackMove = NewObject<UInputAction>(this, TEXT("IA_AttackMove"));
	IA_AttackMove->ValueType = EInputActionValueType::Boolean;

	// ---- MappingContext 생성 ----
	IMC_Selection = NewObject<UInputMappingContext>(this, TEXT("IMC_Selection"));

	IMC_Selection->MapKey(IA_Select, EKeys::LeftMouseButton);
	IMC_Selection->MapKey(IA_SelectHero1, EKeys::One);
	IMC_Selection->MapKey(IA_SelectHero2, EKeys::Two);
	IMC_Selection->MapKey(IA_SelectHero3, EKeys::Three);
	IMC_Selection->MapKey(IA_MoveCommand, EKeys::RightMouseButton);
	IMC_Selection->MapKey(IA_AttackMove, EKeys::A);

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
		EIC->BindAction(IA_MoveCommand, ETriggerEvent::Started, this, &ThisClass::HandleMoveCommand);
		EIC->BindAction(IA_AttackMove, ETriggerEvent::Started, this, &ThisClass::HandleAttackMovePressed);
	}
}

void AHRBPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// 마우스 좌클릭이 눌려있는 동안 현재 마우스 위치 갱신 (드래그 추적)
	// 공격이동 모드에서는 드래그 추적 안 함
	if (!bAttackMoveMode && IsInputKeyDown(EKeys::LeftMouseButton))
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
	// 공격이동 모드면 선택 시작을 무시 (HandleSelectCompleted에서 처리)
	if (bAttackMoveMode)
	{
		return;
	}

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
	// 공격이동 모드면 확정 처리
	if (bAttackMoveMode)
	{
		HandleAttackMoveConfirm();
		return;
	}

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
		AHRBHeroCharacter* HitHero = Cast<AHRBHeroCharacter>(HitResult.GetActor());
		// 적 영웅은 선택 불가 (공격 대상이므로)
		if (HitHero && !HitHero->IsA<AHRBEnemyHeroCharacter>())
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

void AHRBPlayerController::HandleMoveCommand(const FInputActionValue& Value)
{
	// 공격이동 모드 취소
	if (bAttackMoveMode)
	{
		bAttackMoveMode = false;
		CurrentMouseCursor = EMouseCursor::Default;
		UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] AttackMove mode cancelled by right-click"));
	}

	if (SelectedHeroes.Num() == 0)
	{
		return;
	}

	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Pawn, false, HitResult))
	{
		// 적 영웅을 클릭했으면 공격 명령
		if (AHRBEnemyHeroCharacter* EnemyTarget = Cast<AHRBEnemyHeroCharacter>(HitResult.GetActor()))
		{
			CommandAttack(EnemyTarget);
			return;
		}
	}

	// 적이 아니면 이동 명령
	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		const FVector Destination = HitResult.Location;

		// 선택된 모든 영웅에게 이동 명령
		for (AHRBHeroCharacter* Hero : SelectedHeroes)
		{
			if (Hero)
			{
				Hero->MoveToLocation(Destination);
			}
		}

		// 이동 마커 스폰
		SpawnMoveMarker(Destination);

		UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] Move command: %d heroes -> (%.0f, %.0f, %.0f)"),
			SelectedHeroes.Num(), Destination.X, Destination.Y, Destination.Z);
	}
}

// ==================== Attack Move ====================

void AHRBPlayerController::HandleAttackMovePressed(const FInputActionValue& Value)
{
	if (SelectedHeroes.Num() == 0)
	{
		return;
	}

	bAttackMoveMode = true;
	CurrentMouseCursor = EMouseCursor::Crosshairs;
	UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] AttackMove mode activated (press LMB to confirm)"));
}

void AHRBPlayerController::HandleAttackMoveConfirm()
{
	// 공격이동 모드 해제
	bAttackMoveMode = false;
	CurrentMouseCursor = EMouseCursor::Default;

	if (SelectedHeroes.Num() == 0)
	{
		return;
	}

	// 커서 아래 적이 있는지 확인
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Pawn, false, HitResult))
	{
		if (AHRBEnemyHeroCharacter* EnemyTarget = Cast<AHRBEnemyHeroCharacter>(HitResult.GetActor()))
		{
			// 적 클릭 → 직접 공격 명령
			CommandAttack(EnemyTarget);
			UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] AttackMove: direct attack on enemy"));
			return;
		}
	}

	// 땅 클릭 → 공격이동 명령
	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		const FVector Destination = HitResult.Location;

		for (AHRBHeroCharacter* Hero : SelectedHeroes)
		{
			if (Hero && !Hero->bIsDead)
			{
				Hero->AttackMoveToLocation(Destination);
			}
		}

		// 주황색 마커 스폰
		SpawnAttackMoveMarker(Destination);

		UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] AttackMove command: %d heroes -> (%.0f, %.0f, %.0f)"),
			SelectedHeroes.Num(), Destination.X, Destination.Y, Destination.Z);
	}
}

void AHRBPlayerController::SpawnAttackMoveMarker(const FVector& Location)
{
	// SpawnMoveMarker와 동일하되, 주황색으로 변경
	SpawnMoveMarker(Location);
	if (IsValid(CurrentMoveMarker))
	{
		CurrentMoveMarker->SetMarkerColor(FLinearColor(1.0f, 0.3f, 0.0f, 1.0f)); // 주황색
	}
}

// ==================== Command Attack ====================

void AHRBPlayerController::CommandAttack(AHRBEnemyHeroCharacter* Target)
{
	if (!Target || Target->bIsDead)
	{
		return;
	}

	// 이전 타겟의 타겟 데칼 해제
	if (PreviousAttackTarget && IsValid(PreviousAttackTarget))
	{
		PreviousAttackTarget->SetTargeted(false);
	}

	// 새 타겟에 타겟 데칼 표시
	Target->SetTargeted(true);
	PreviousAttackTarget = Target;

	for (AHRBHeroCharacter* Hero : SelectedHeroes)
	{
		if (Hero && !Hero->bIsDead)
		{
			// 사거리 내면 즉시 공격, 아니면 먼저 이동
			const float Distance = FVector::Dist(Hero->GetActorLocation(), Target->GetActorLocation());
			if (Distance <= Hero->AttackRange)
			{
				Hero->Attack(Target);
			}
			else
			{
				// 타겟 위치로 이동 후 공격은 추후 자동전투에서 처리
				Hero->MoveToLocation(Target->GetActorLocation());
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[HRBPlayerController] Attack command: %d heroes -> Enemy %d"),
		SelectedHeroes.Num(), Target->HeroIndex);
}

void AHRBPlayerController::SpawnMoveMarker(const FVector& Location)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 이전 마커가 아직 살아있으면 제거
	if (IsValid(CurrentMoveMarker))
	{
		CurrentMoveMarker->Destroy();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentMoveMarker = World->SpawnActor<AHRBMoveMarker>(
		AHRBMoveMarker::StaticClass(),
		Location + FVector(0.0f, 0.0f, 5.0f), // 지면보다 약간 위
		FRotator::ZeroRotator,
		SpawnParams);
}
