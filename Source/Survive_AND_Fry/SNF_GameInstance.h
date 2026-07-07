#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SNF_GameInstance.generated.h"

/*
* Game instance that owns save game persistence and cross-level state.
* LoadSaveGame runs automatically on Init. 
* SaveGame records the game mode's elapsed match time
* when it beats the stored score.
*/
UCLASS()
class SURVIVE_AND_FRY_API USNF_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Loads the save slot on startup
	virtual void Init() override;

	// Loads the save slot into SaverSubClass and the GI variables
	// Creates and writes a defaulted save if the slot does not exist
	// A stored HighScore of 0 is treated as uninitialized and reset to the default
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void LoadSaveGame();

	// Records the current run if it beats the stored high score,
	// otherwise refreshes the mirrors from disk via LoadSaveGame
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void SaveGame();

	// Overwrites the save slot with default values and refreshes the mirrors
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void ResetSaveGame();

	// Cached save game object for the active slot
	UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
	class USNF_SaveGame* SaverSubClass;

	// Mirror of the stored high score for HUD and menu bindings
	UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
	int32 Highscore_GI = 0;

	// Mirror of the stored difficulty for HUD and menu bindings
	UPROPERTY(BlueprintReadWrite, Category = "SaveGame")
	FString DifficultyMode_GI;

	// Difficulty currently selected in the menu
	// Written into the save on a new record
	UPROPERTY(BlueprintReadWrite, Category = "Session")
	FString DifficultyMode;

	// Starting time for the next match, set by the menu based on difficulty
	// The game mode pulls this on BeginPlay when it is greater than zero
	UPROPERTY(BlueprintReadWrite, Category = "Session")
	int32 TimeLeft = 0;

private:
	// Returns true when the new score should replace the stored one
	bool IsNewRecord(int32 NewScore, int32 CurrentBest) const;

	// Loads the existing slot or creates a fresh save object
	void EnsureSaveObject();

	// Copies the save object's values into the GI mirror variables
	void RefreshMirrors();
};
