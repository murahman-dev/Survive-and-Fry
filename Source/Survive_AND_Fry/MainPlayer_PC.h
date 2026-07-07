#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayer_PC.generated.h"

/*
* Player controller responsible for HUD management, the pause menu,
* and the end-of-match screens.
* Match rules (timer, zombie counters, win/lose) live in ASNF_GameMode,
* which calls ShowGameWinScreen or ShowGameOverScreen when the match ends.
*/
UCLASS()
class SURVIVE_AND_FRY_API AMainPlayer_PC : public APlayerController
{
	GENERATED_BODY()

public:
	// Current task description displayed on the HUD
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString TaskDescription;

	// Builds the HUD task string dynamically from the order parameters
	// Called by AServingDesk::PickNewOrder whenever a new order is generated
	// Scale is the required sandwich scale (1, 2, or 3)
	// DisplayName is the ingredient display name from the DataTable (e.g., "Tomato Sandwich")
	void SetTaskDescription(int32 Scale, const FString& DisplayName);

	// Called by the game mode when the player wins
	// Disables input and displays the win screen
	void ShowGameWinScreen();

	// Called by the game mode when the player loses
	// Disables input and displays the game over screen
	void ShowGameOverScreen();

	// Pauses the game, shows the pause menu, and switches input to UI only
	UFUNCTION(BlueprintCallable)
	void PauseGame();

	// Removes the pause menu, restores game input, and unpauses
	UFUNCTION(BlueprintCallable)
	void ResumeGame();

protected:
	virtual void BeginPlay() override;

	// Binds the Pause action
	virtual void SetupInputComponent() override;

private:
	// Widget class for the main gameplay HUD
	// Spawned and added to viewport on BeginPlay
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	// Widget class displayed when the player loses (time runs out)
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	// Widget class displayed when the player wins (enough zombies saved)
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> GameWinWidgetClass;

	// Camera actor class used for the top-down view
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf <class AActor> WorldCameraClass;

	// Widget class for the pause menu
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	// Created once on first pause and reused afterwards
	UPROPERTY()
	UUserWidget* PauseMenuWidget;

	// Toggle handler for the Pause input action
	void TogglePause();

	UPROPERTY()
	UUserWidget* GameWinWidget;

	UPROPERTY()
	UUserWidget* GameOverWidget;
};
