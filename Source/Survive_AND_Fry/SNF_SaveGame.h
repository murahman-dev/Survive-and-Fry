#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SNF_SaveGame.generated.h"

/*
* Save game object saved to disk in the "SNFSaveGame" slot.
* Stores the best score and the difficulty it was achieved on.
*/
UCLASS()
class SURVIVE_AND_FRY_API USNF_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// Best recorded score
	// Defaults to the guard value so any real run beats an empty save
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HighScore = 9999;

	// Difficulty the high score was achieved on
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DifficultyMode;
};
