#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "IngredientData.generated.h"

/*
* DataTable row struct defining a single ingredient that the game can request.
* Each row represents one possible order ingredient (e.g., Tomato, Lettuce, Onion).
* Adding a new ingredient to the game requires only a new row in the DataTable asset,
* no code changes are needed in ServingDesk, MainPlayer_PC, or validation logic.
* IngredientName values are case-sensitive FNames and must exactly match the
* IngredientName set on the corresponding chopped item Blueprint.
*/
USTRUCT(BlueprintType)
struct FIngredientRow : public FTableRowBase
{
	GENERATED_BODY()

	// Internal identifier matched against ABread::OwnedIngredients during order validation
	// Must exactly match the IngredientName on the corresponding AItem/Blueprint (e.g., "Tomato")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName IngredientName;

	// Display name shown in the HUD task description (e.g., "Tomato Sandwich")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
};
