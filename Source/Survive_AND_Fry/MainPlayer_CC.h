#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer_CC.generated.h"

/*
* Player character that handles all gameplay interactions in the kitchen.
* Uses a sphere sweep trace from the PlayerTracePointer arrow component to detect
* desks in front of the player. 
* The trace runs once per frame in Tick, and the resulting desk is cached in 
* HighlightedDesk for all input handlers to consume.
* Grab/Release picks up items from desks or places held items onto desks,
* attempting to combine when both the player and desk have items.
* Chopping runs while the Chop input is held at a ChoppingDesk.
* Serving submits the sandwich at a ServingDesk.
* Scaling resizes the bread (1x-3x) via the ScaleUp/ScaleDown input actions.
*/
UCLASS()
class SURVIVE_AND_FRY_API AMainPlayer_CC : public ACharacter
{
	GENERATED_BODY()

public:
	AMainPlayer_CC();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// The item the player is currently carrying (null when hands are empty)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	AActor* HoldingItem;

	// Current chopping progress (0.0 to 1.0)
	// Bound to the progress bar widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Progress;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool IsHolding = false;

	// True while the player is actively chopping
	// Prevents grab/release during the animation
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool IsChopping = false;

	// Updates the HUD display showing the current bread scale
	void SetTaskDescription(int32 CurrentScale);

private:
	// Maximum distance of the interaction sphere sweep from the PlayerTracePointer
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float TraceDistance = 30.f;

	// Radius of the interaction sphere sweep
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float SphereSize = 15.f;

	// Knife mesh on the player character
	// Visible only while actively chopping
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* Knife;

	// Transform where held items are attached on the character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class USceneComponent* HoldingLocation;

	// Arrow component defining the origin and direction of the interaction trace
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UArrowComponent* PlayerTracePointer;

	// Sound effect played during chopping
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USoundBase* ChoppingSound;

	// Character max walk speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float MoveSpeed = 300.f;

	// HUD-bound string showing the current bread scale (e.g., "CURRENT SCALE\n2x")
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FString CurrentScaleDescription;

	// Desk currently hit by the per-frame trace
	// Consumed by all input handlers and used to clear the highlight when the trace moves on
	UPROPERTY()
	class AItemDesk* HighlightedDesk;

	// Desk the player is currently chopping at
	// Used to restore its knife when chopping stops
	UPROPERTY()
	class AChoppingDesk* CurrentChoppingDesk;

	// Reusable audio component for the chopping sound
	UPROPERTY()
	class UAudioComponent* ChopAudioComponent;

	// True while the Chop input action is held
	// Read by ProcessChopping on each timer tick
	bool bChopHeld = false;

	// Interval of the chopping timer
	static constexpr float ChoppingInterval = 0.3f;

	// Repeating timer (every 0.3s) that drives the chopping check instead of per-frame updates
	FTimerHandle ChoppingHandle;

	// Applies forward movement input
	void MoveForward(float AxisValue);

	// Applies right movement input
	void MoveRight(float AxisValue);

	// Input handlers for the Chop action
	void ChopPressed();
	void ChopReleased();

	// Main interaction handler bound to the Grab/Release input action
	void GrabORRelease();

	// Timer callback that checks if the player is facing a ChoppingDesk
	// If so, delegates to ProcessChopping to advance the chop
	// If the player has walked away mid-chop, resets the chopping state so interactions aren't blocked
	void StartChopping();

	// Performs a sphere sweep from the PlayerTracePointer, toggles desk highlighting,
	// and caches the hit desk in HighlightedDesk for the input handlers
	void TraceObject();

	// Handles all grab, place, and combine logic for desk interactions
	// If both the player and desk have items, attempts to combine them via AItem::CombineItems
	// If only the desk has an item, the player picks it up
	// If only the player has an item, it is placed on the desk
	void DeskFunctions(class AItemDesk* Desk);

	// Manages the chopping state: toggles knife visibility between the desk and player,
	// plays the chopping sound, and advances the vegetable's chop progress
	void ProcessChopping(class AChoppingDesk* Desk);

	// Resets the chopping state: restores knife visibility, stops the sound, clears IsChopping
	void StopChopping();

	// Sends the serve command to the ServingDesk the player is currently facing
	void ProcessServing();

	void ScaleUp();

	void ScaleDown();

	// Shared scaling logic for ScaleUp/ScaleDown
	// Clamps the scale between 1 and 3 and updates the HUD and the bread's actual scale
	void AdjustScale(int32 Direction);
};
