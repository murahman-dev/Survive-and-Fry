#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Vegetable.generated.h"

/*
* Whole vegetable item that can be placed on a ChoppingDesk and chopped by the player.
* Displays a progress bar widget above the item during chopping. 
* When progress reaches 1.0, the vegetable destroys itself,
* and spawns the ChoppedItemReference Blueprint in its place.
*/
UCLASS()
class SURVIVE_AND_FRY_API AVegetable : public AItem
{
	GENERATED_BODY()
public:
	AVegetable();

public:
	virtual void BeginPlay() override;

	// Called each tick while the player holds the chop input at a ChoppingDesk
	// Advances Progress based on Delta and ChoppingSpeed
	// When Progress reaches 1.0, spawns the chopped item and destroys this vegetable
	virtual void ChopItem(float Delta) override;

private:
	// Multiplier for chopping speed
	// Higher values fill the progress bar faster
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float ChoppingSpeed = 1.2f;

	// Widget component displaying the chopping progress bar above the vegetable
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UWidgetComponent* ProgressWidgetComponent;

	// Blueprint class to spawn when chopping completes (e.g., BP_ChoppedTomato)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> ChoppedItemReference;

	UPROPERTY()
	class AMainPlayer_CC* MainPlayer;
};
