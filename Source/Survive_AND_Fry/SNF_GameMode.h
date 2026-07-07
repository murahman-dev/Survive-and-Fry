#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SNF_GameMode.generated.h"

/*
* Game mode that owns the match rules: the countdown timer, the zombie counters,
* the serve scoring, and the win/lose decision.
* On BeginPlay the starting time is pulled from the game instance when the menu
* has set one.
* On a win the elapsed match time is submitted to the game instance's save system.
*/
UCLASS()
class SURVIVE_AND_FRY_API ASNF_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Countdown timer in seconds, decremented each second
	// Adjusted by RegisterServeResult on serve attempts
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TimeLeft = 120;

	// Number of correct sandwiches served
	// Incremented by RegisterServeResult on successful validation
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ZombiesSaved = 0;

	// Target number of zombies to save for a win condition
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ZombiesNeedToBeSaved = 20;

	// Seconds the match has been running
	// Counted separately from TimeLeft so serve bonuses and penalties
	// don't distort it, recorded as the score on a win
	UPROPERTY(BlueprintReadOnly)
	int32 ElapsedTime = 0;

	// Applies the score and timer rules for a serve attempt
	// Success: +1 zombie saved, +5 seconds
	// Failure: -10 seconds
	UFUNCTION(BlueprintCallable)
	void RegisterServeResult(bool bSuccess);

protected:
	virtual void BeginPlay() override;

private:
	// Repeating timer that fires every second to decrement TimeLeft
	// and check end-game conditions
	FTimerHandle WaveTimer;

	// Timer callback
	// Decrements TimeLeft and ends the match when conditions are met
	void WaveTimerDelegate();

	// Ends the match: pauses the timer, submits the score on a win
	// Tells the player controller which end screen to show
	void EndMatch(bool bPlayerWon);
};
