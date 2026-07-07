#include "SNF_GameMode.h"
#include "SNF_GameInstance.h"
#include "MainPlayer_PC.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ASNF_GameMode::BeginPlay()
{
	Super::BeginPlay();

	// The menu writes the difficulty's starting time into the game instance
	if (USNF_GameInstance* GameInstance = Cast<USNF_GameInstance>(GetGameInstance()))
	{
		if (GameInstance->TimeLeft > 0)
		{
			TimeLeft = GameInstance->TimeLeft;
		}
	}

	// Start the game timer
	// Ticks every second to decrement TimeLeft and check win/lose
	GetWorldTimerManager().SetTimer(WaveTimer, this, &ASNF_GameMode::WaveTimerDelegate, 1.f, true);
}

void ASNF_GameMode::RegisterServeResult(bool bSuccess)
{
	if (bSuccess)
	{
		ZombiesSaved += 1;
		TimeLeft += 5;
	}
	else
	{
		// Clamped at 0 so the HUD never shows negative time before game over fires
		TimeLeft = FMath::Max(TimeLeft - 10, 0);
	}
}

void ASNF_GameMode::WaveTimerDelegate()
{
	ElapsedTime += 1;
	TimeLeft -= 1;

	if (ZombiesSaved >= ZombiesNeedToBeSaved)
	{
		EndMatch(true);
	}
	else if (TimeLeft <= 0)
	{
		EndMatch(false);
	}
}

void ASNF_GameMode::EndMatch(bool bPlayerWon)
{
	GetWorldTimerManager().PauseTimer(WaveTimer);

	// Record the run before the controller takes over
	if (bPlayerWon)
	{
		if (USNF_GameInstance* GameInstance = Cast<USNF_GameInstance>(GetGameInstance()))
		{
			GameInstance->SaveGame();
		}
	}

	APlayerController* PlayerControllerReference = UGameplayStatics::GetPlayerController(this, 0);
	if (AMainPlayer_PC* MainPlayer_PC = Cast<AMainPlayer_PC>(PlayerControllerReference))
	{
		if (bPlayerWon)
		{
			MainPlayer_PC->ShowGameWinScreen();
		}
		else
		{
			MainPlayer_PC->ShowGameOverScreen();
		}
	}
}
