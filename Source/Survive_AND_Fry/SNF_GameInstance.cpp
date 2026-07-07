#include "SNF_GameInstance.h"
#include "SNF_SaveGame.h"
#include "SNF_GameMode.h"
#include "Kismet/GameplayStatics.h"

// Slot identity shared by all three save functions
// Must stay "SNFSaveGame" so existing player saves keep loading
static const FString SaveSlotName = TEXT("SNFSaveGame");
static constexpr int32 SaveUserIndex = 0;

// Default written into fresh or reset saves
// Acts as a guard that any real run should beat
static constexpr int32 DefaultHighScore = 9999;
static const FString DefaultDifficultyMode = TEXT("");

void USNF_GameInstance::Init()
{
	Super::Init();

	LoadSaveGame();
}

bool USNF_GameInstance::IsNewRecord(int32 NewScore, int32 CurrentBest) const
{
	return NewScore < CurrentBest;
}

void USNF_GameInstance::EnsureSaveObject()
{
	if (SaverSubClass != nullptr)
	{
		return;
	}

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		SaverSubClass = Cast<USNF_SaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	}

	// Fall through to creation when no slot exists
	if (SaverSubClass == nullptr)
	{
		SaverSubClass = Cast<USNF_SaveGame>(UGameplayStatics::CreateSaveGameObject(USNF_SaveGame::StaticClass()));
	}
}

void USNF_GameInstance::RefreshMirrors()
{
	if (SaverSubClass != nullptr)
	{
		Highscore_GI = SaverSubClass->HighScore;
		DifficultyMode_GI = SaverSubClass->DifficultyMode;
	}
}

void USNF_GameInstance::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		SaverSubClass = Cast<USNF_SaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));

		// A stored HighScore of 0 is treated as an uninitialized save
		// Reset it to the default guard value and write it back
		if (SaverSubClass != nullptr && SaverSubClass->HighScore == 0)
		{
			SaverSubClass->HighScore = DefaultHighScore;
			SaverSubClass->DifficultyMode = DefaultDifficultyMode;
			UGameplayStatics::SaveGameToSlot(SaverSubClass, SaveSlotName, SaveUserIndex);
		}

		RefreshMirrors();
	}

	// No slot on disk or the cast failed
	// Create a defaulted save and write it
	if (SaverSubClass == nullptr)
	{
		SaverSubClass = Cast<USNF_SaveGame>(UGameplayStatics::CreateSaveGameObject(USNF_SaveGame::StaticClass()));
		if (SaverSubClass != nullptr)
		{
			SaverSubClass->HighScore = DefaultHighScore;
			SaverSubClass->DifficultyMode = DefaultDifficultyMode;
			RefreshMirrors();

			const bool bSaved = UGameplayStatics::SaveGameToSlot(SaverSubClass, SaveSlotName, SaveUserIndex);
			UE_LOG(LogTemp, Display, TEXT("Created new save slot: %s"), bSaved ? TEXT("success") : TEXT("failed"));
		}
	}
}

void USNF_GameInstance::SaveGame()
{
	EnsureSaveObject();
	if (SaverSubClass == nullptr)
	{
		return;
	}

	// The score for a run is the total elapsed match time
	// Tracked by the game mode independently of TimeLeft
	ASNF_GameMode* GameMode = nullptr;
	if (UWorld* World = GetWorld())
	{
		GameMode = World->GetAuthGameMode<ASNF_GameMode>();
	}

	if (GameMode == nullptr)
	{
		return;
	}

	const int32 NewScore = GameMode->ElapsedTime;

	if (IsNewRecord(NewScore, SaverSubClass->HighScore))
	{
		SaverSubClass->HighScore = NewScore;
		SaverSubClass->DifficultyMode = DifficultyMode;
		RefreshMirrors();

		const bool bSaved = UGameplayStatics::SaveGameToSlot(SaverSubClass, SaveSlotName, SaveUserIndex);
		UE_LOG(LogTemp, Display, TEXT("New high score saved: %s"), bSaved ? TEXT("success") : TEXT("failed"));
	}
	else
	{
		LoadSaveGame();
	}
}

void USNF_GameInstance::ResetSaveGame()
{
	EnsureSaveObject();
	if (SaverSubClass == nullptr)
	{
		return;
	}

	SaverSubClass->HighScore = DefaultHighScore;
	SaverSubClass->DifficultyMode = DefaultDifficultyMode;
	RefreshMirrors();

	const bool bSaved = UGameplayStatics::SaveGameToSlot(SaverSubClass, SaveSlotName, SaveUserIndex);
	UE_LOG(LogTemp, Display, TEXT("Save game reset: %s"), bSaved ? TEXT("success") : TEXT("failed"));
}
