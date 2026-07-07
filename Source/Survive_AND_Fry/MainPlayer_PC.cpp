#include "MainPlayer_PC.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"

void AMainPlayer_PC::SetTaskDescription(int32 Scale, const FString& DisplayName)
{
	TaskDescription = FString::Printf(TEXT("TASK\n%dx %s"), Scale, *DisplayName);
}

void AMainPlayer_PC::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(false);

	// Create and display the main gameplay HUD
	UUserWidget* HUDWidget = CreateWidget(this, HUDWidgetClass);

	if (HUDWidget != nullptr)
	{
		HUDWidget->AddToViewport();
	}

	// Blend to the level's top-down camera over 3 seconds
	AActor* CameraActor = UGameplayStatics::GetActorOfClass(GetWorld(), WorldCameraClass);

	if (CameraActor != nullptr)
	{
		SetViewTargetWithBlend(CameraActor, 3.f, EViewTargetBlendFunction::VTBlend_Linear);
	}
}

void AMainPlayer_PC::ShowGameWinScreen()
{
	DisableInput(this);

	GameWinWidget = CreateWidget(this, GameWinWidgetClass);

	if (GameWinWidget != nullptr)
	{
		SetShowMouseCursor(true);
		GameWinWidget->AddToViewport();
	}
}

void AMainPlayer_PC::SetupInputComponent()
{
	Super::SetupInputComponent();

	FInputActionBinding& PauseBinding = InputComponent->BindAction(TEXT("Pause"), EInputEvent::IE_Pressed, this, &AMainPlayer_PC::TogglePause);

	// Must execute while paused or the key can never resume
	PauseBinding.bExecuteWhenPaused = true;
}

void AMainPlayer_PC::TogglePause()
{
	if (IsPaused())
	{
		ResumeGame();
	}
	else
	{
		PauseGame();
	}
}

void AMainPlayer_PC::PauseGame()
{
	if (IsPaused())
	{
		return;
	}

	// Created once on first pause and reused on later pauses
	if (PauseMenuWidget == nullptr && PauseMenuWidgetClass != nullptr)
	{
		PauseMenuWidget = CreateWidget(this, PauseMenuWidgetClass);
	}

	if (PauseMenuWidget == nullptr)
	{
		return;
	}

	PauseMenuWidget->AddToViewport();

	// UI-only input with focus on the menu
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	FlushPressedKeys();

	SetShowMouseCursor(true);
	SetPause(true);
}

void AMainPlayer_PC::ResumeGame()
{
	if (IsPaused() == false)
	{
		return;
	}

	if (PauseMenuWidget != nullptr)
	{
		PauseMenuWidget->RemoveFromParent();
	}

	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
	SetPause(false);
}

void AMainPlayer_PC::ShowGameOverScreen()
{
	DisableInput(this);

	GameOverWidget = CreateWidget(this, GameOverWidgetClass);

	if (GameOverWidget != nullptr)
	{
		SetShowMouseCursor(true);
		GameOverWidget->AddToViewport();
	}
}
