#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

/*
* Base class for all interactive items in the game (bread, vegetables, antidote).
* Provides the shared interface for combining items and chopping, as well as
* the IngredientName property used for data-driven identification, removing
* the need for type-specific casts throughout the codebase.
*/
UCLASS()
class SURVIVE_AND_FRY_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();

protected:
	virtual void BeginPlay() override;

public:
	// Override in subclasses to define item combination behavior (e.g., ABread adds ingredients)
	virtual void CombineItems(class AMainPlayer_CC* MainPlayer, class AActor* SecondItem);

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootSceneComponent;

	// When true, the chopping system will process this item at a ChoppingDesk
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool CanBeChopped = false;

	// Identifies this item for data-driven logic
	// Set in Blueprint defaults or subclass constructors
	// Used by ABread::CombineItems to determine which ingredient is being added,
	// and by AServingDesk::ValidateServe to check the served sandwich against the current order
	// Examples: "Tomato", "Lettuce", "Onion", "AntiDote"
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName IngredientName;

	// Chopping progress from 0.0 to 1.0
	// Driven by AVegetable::ChopItem and displayed by the progress widget
	float Progress = 0.f;

	// Override in AVegetable to advance chopping progress
	virtual void ChopItem(float Delta);
};
