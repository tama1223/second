#include "HeroRoundBattle/Core/HRBPlayerController.h"
#include "HeroRoundBattle/Core/HRBPlayerState.h"
#include "HeroRoundBattle/Core/HRBGameState.h"
#include "HeroRoundBattle/Hero/HRBHeroCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EngineUtils.h"

AHRBPlayerController::AHRBPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AHRBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	// Set up top-down camera
	CameraLocation = FVector(0.f, 0.f, CameraHeight);

	// Input mode: game and UI (mouse visible, game input active)
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AHRBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Create input actions
	IA_LeftClick = NewObject<UInputAction>(this, TEXT("IA_LeftClick"));
	IA_LeftClick->ValueType = EInputActionValueType::Boolean;

	IA_RightClick = NewObject<UInputAction>(this, TEXT("IA_RightClick"));
	IA_RightClick->ValueType = EInputActionValueType::Boolean;

	IA_Select1 = NewObject<UInputAction>(this, TEXT("IA_Select1"));
	IA_Select1->ValueType = EInputActionValueType::Boolean;

	IA_Select2 = NewObject<UInputAction>(this, TEXT("IA_Select2"));
	IA_Select2->ValueType = EInputActionValueType::Boolean;

	IA_Select3 = NewObject<UInputAction>(this, TEXT("IA_Select3"));
	IA_Select3->ValueType = EInputActionValueType::Boolean;

	IA_SelectAll = NewObject<UInputAction>(this, TEXT("IA_SelectAll"));
	IA_SelectAll->ValueType = EInputActionValueType::Boolean;

	IA_Scroll = NewObject<UInputAction>(this, TEXT("IA_Scroll"));
	IA_Scroll->ValueType = EInputActionValueType::Axis1D;

	// Create mapping context
	IMC_RTS = NewObject<UInputMappingContext>(this, TEXT("IMC_RTS"));
	IMC_RTS->MapKey(IA_LeftClick, EKeys::LeftMouseButton);
	IMC_RTS->MapKey(IA_RightClick, EKeys::RightMouseButton);
	IMC_RTS->MapKey(IA_Select1, EKeys::One);
	IMC_RTS->MapKey(IA_Select2, EKeys::Two);
	IMC_RTS->MapKey(IA_Select3, EKeys::Three);
	IMC_RTS->MapKey(IA_SelectAll, EKeys::A);  // Ctrl+A handled via modifier later; plain A for MVP
	IMC_RTS->MapKey(IA_Scroll, EKeys::MouseWheelAxis);

	// Register mapping context
	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(IMC_RTS, 0);
	}

	// Bind actions
	if (auto* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(IA_LeftClick, ETriggerEvent::Started, this, &AHRBPlayerController::HandleLeftClick);
		EIC->BindAction(IA_RightClick, ETriggerEvent::Started, this, &AHRBPlayerController::HandleRightClick);
		EIC->BindAction(IA_Select1, ETriggerEvent::Started, this, &AHRBPlayerController::HandleSelectHero1);
		EIC->BindAction(IA_Select2, ETriggerEvent::Started, this, &AHRBPlayerController::HandleSelectHero2);
		EIC->BindAction(IA_Select3, ETriggerEvent::Started, this, &AHRBPlayerController::HandleSelectHero3);
		EIC->BindAction(IA_SelectAll, ETriggerEvent::Started, this, &AHRBPlayerController::HandleSelectAll);
		EIC->BindAction(IA_Scroll, ETriggerEvent::Triggered, this, &AHRBPlayerController::HandleScroll);
	}
}

void AHRBPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocalController()) return;

	// Update camera - simple top-down view
	FVector DesiredLocation = CameraLocation;
	FRotator DesiredRotation(CameraPitch, 0.f, 0.f);

	SetViewTarget(nullptr);

	// Directly set camera
	FMinimalViewInfo ViewInfo;
	ViewInfo.Location = DesiredLocation;
	ViewInfo.Rotation = DesiredRotation;
	ViewInfo.FOV = 90.f;

	// Edge panning
	float MouseX, MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		int32 ViewportX, ViewportY;
		GetViewportSize(ViewportX, ViewportY);

		const float EdgeThreshold = 20.f;
		FVector PanDir = FVector::ZeroVector;

		if (MouseX < EdgeThreshold) PanDir.Y -= 1.f;
		if (MouseX > ViewportX - EdgeThreshold) PanDir.Y += 1.f;
		if (MouseY < EdgeThreshold) PanDir.X += 1.f;
		if (MouseY > ViewportY - EdgeThreshold) PanDir.X -= 1.f;

		CameraLocation += PanDir * CameraEdgePanSpeed * DeltaTime;
	}
}

// --- Input Handlers ---

void AHRBPlayerController::HandleLeftClick()
{
	// Select hero under cursor
	AHRBHeroCharacter* HeroUnderCursor = GetHeroUnderCursor();
	if (HeroUnderCursor && HeroUnderCursor->GetOwningPlayer() == GetMyPlayerIndex())
	{
		SelectHero(HeroUnderCursor);
	}
}

void AHRBPlayerController::HandleRightClick()
{
	// Check if clicking on an enemy -> attack command
	AHRBHeroCharacter* TargetHero = GetHeroUnderCursor();

	if (TargetHero && TargetHero->GetOwningPlayer() != GetMyPlayerIndex() && !TargetHero->IsDead())
	{
		// Attack command
		for (auto& WeakHero : SelectedHeroes)
		{
			if (WeakHero.IsValid())
			{
				Server_CommandAttack(WeakHero.Get(), TargetHero);
			}
		}
	}
	else
	{
		// Move command
		FVector WorldLoc;
		if (GetMouseWorldLocation(WorldLoc))
		{
			for (auto& WeakHero : SelectedHeroes)
			{
				if (WeakHero.IsValid())
				{
					Server_CommandMoveTo(WeakHero.Get(), WorldLoc);
				}
			}
		}
	}
}

void AHRBPlayerController::HandleSelectHero1() { SelectHeroByIndex(0); }
void AHRBPlayerController::HandleSelectHero2() { SelectHeroByIndex(1); }
void AHRBPlayerController::HandleSelectHero3() { SelectHeroByIndex(2); }
void AHRBPlayerController::HandleSelectAll() { SelectAllHeroes(); }

void AHRBPlayerController::HandleScroll(const FInputActionValue& Value)
{
	float Axis = Value.Get<float>();
	CameraLocation.Z = FMath::Clamp(
		CameraLocation.Z - Axis * CameraScrollSpeed * 50.f,
		CameraMinHeight,
		CameraMaxHeight
	);
}

// --- Server RPCs ---

void AHRBPlayerController::Server_CommandMoveTo_Implementation(AHRBHeroCharacter* Hero, FVector Destination)
{
	if (!Hero || Hero->GetOwningPlayer() != GetMyPlayerIndex()) return;
	Hero->CommandMoveTo(Destination);
}

void AHRBPlayerController::Server_CommandAttack_Implementation(AHRBHeroCharacter* Hero, AHRBHeroCharacter* Target)
{
	if (!Hero || Hero->GetOwningPlayer() != GetMyPlayerIndex()) return;
	Hero->CommandAttack(Target);
}

void AHRBPlayerController::Server_CommandStop_Implementation(AHRBHeroCharacter* Hero)
{
	if (!Hero || Hero->GetOwningPlayer() != GetMyPlayerIndex()) return;
	Hero->CommandStop();
}

// --- Helpers ---

bool AHRBPlayerController::GetMouseWorldLocation(FVector& OutLocation) const
{
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		OutLocation = HitResult.Location;
		return true;
	}
	return false;
}

AHRBHeroCharacter* AHRBPlayerController::GetHeroUnderCursor() const
{
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Pawn, false, HitResult))
	{
		return Cast<AHRBHeroCharacter>(HitResult.GetActor());
	}
	return nullptr;
}

void AHRBPlayerController::SelectHero(AHRBHeroCharacter* Hero)
{
	SelectedHeroes.Empty();
	if (Hero)
	{
		SelectedHeroes.Add(Hero);
	}
}

void AHRBPlayerController::SelectHeroByIndex(int32 Index)
{
	TArray<AHRBHeroCharacter*> MyHeroes = GetMyHeroes();
	if (MyHeroes.IsValidIndex(Index))
	{
		SelectHero(MyHeroes[Index]);
	}
}

void AHRBPlayerController::SelectAllHeroes()
{
	SelectedHeroes.Empty();
	for (AHRBHeroCharacter* Hero : GetMyHeroes())
	{
		if (Hero && !Hero->IsDead())
		{
			SelectedHeroes.Add(Hero);
		}
	}
}

TArray<AHRBHeroCharacter*> AHRBPlayerController::GetMyHeroes() const
{
	TArray<AHRBHeroCharacter*> Result;
	int32 MyIdx = GetMyPlayerIndex();

	for (TActorIterator<AHRBHeroCharacter> It(GetWorld()); It; ++It)
	{
		if ((*It)->GetOwningPlayer() == MyIdx)
		{
			Result.Add(*It);
		}
	}

	// Sort by hero index
	Result.Sort([](const AHRBHeroCharacter& A, const AHRBHeroCharacter& B)
	{
		return A.GetHeroIndex() < B.GetHeroIndex();
	});

	return Result;
}

int32 AHRBPlayerController::GetMyPlayerIndex() const
{
	if (const AHRBPlayerState* PS = GetPlayerState<AHRBPlayerState>())
	{
		return PS->PlayerIndex;
	}
	return -1;
}
