#pragma once

#include "CoreMinimal.h"
#include "ItemDesk.h"
#include "ServingDesk.generated.h"

/*
* Delivery station where the player submits completed sandwiches to save zombies.
* Generates random orders from an IngredientDataTable, each requiring a specific
* ingredient, the antidote, and a specific scale (1x, 2x, or 3x).
* ValidateServe handles both success and failure outcomes in a single function.
* Correct orders add a zombie saved and 5 seconds, wrong orders cost a 10 second penalty.
* After each serve (success or failure), a new random order is generated via PickNewOrder.
* The IngredientDataTable must be assigned in the editor or no orders will be generated.
*/
UCLASS()
class SURVIVE_AND_FRY_API AServingDesk : public AItemDesk
{
	GENERATED_BODY()

public:
	AServingDesk();

	// Called when the player presses the serve input while facing this desk
	void ServeItem();

	// Scales the bread currently on the desk
	void EnlargeItem(int32 ScaleAmount);

	// Current scale level of the bread on this desk (1-3)
	int32 ScaleValue = 1;

	UPROPERTY()
	class ABread* Bread;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* ServingBoard;

	// Sound effect played when the correct order is served
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	class USoundBase* SuccessSound;

	// Sound effect played when the wrong order is served
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	class USoundBase* FailSound;

	// Particle effect activated on a successful serve
	// Deactivates after 3 seconds
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* SuccessfullServeEffect;

	// Particle effect activated on a failed serve
	// Deactivates after 3 seconds
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* UnSuccessfullServeEffect;

	// DataTable of FIngredientRow entries defining all possible order ingredients
	// Must be assigned in the editor
	// If null, PickNewOrder logs an error and no orders are generated
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	class UDataTable* IngredientDataTable;

	UPROPERTY()
	class AMainPlayer_PC* MainPlayer_PC;

	// Owns the serve scoring rules
	UPROPERTY()
	class ASNF_GameMode* GameMode;

	UPROPERTY()
	class AMainPlayer_CC* MainPlayer_CC;

	// Required scale for the current order (1-3)
	// Randomly selected by PickNewOrder
	int32 RandomScale;

	// Required ingredient FName for the current order
	// Matched against ABread::OwnedIngredients
	FName CurrentIngredientName;

	// Display name for the current order, shown in the HUD (e.g., "Tomato Sandwich")
	FString CurrentDisplayName;

	FTimerHandle EffectTimerHandle;

	// Timer callback that deactivates success/failure particle effects after a delay
	void DisableEffects();

	// Validates the served bread against the current order requirements
	// Checks ingredient match, antidote presence, and scale correctness
	// Updates score, adjusts timer, plays the appropriate effect and sound,
	// then generates a new order and destroys the served bread
	void ValidateServe(class ABread* BreadToServe);

	// Selects a random ingredient from the DataTable and a random scale (1-3)
	// Updates the HUD task description via MainPlayer_PC::SetTaskDescription
	void PickNewOrder();
};
