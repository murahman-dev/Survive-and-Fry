#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ChoppedVegetable.generated.h"

/*
* Base class for chopped ingredient items spawned when a vegetable's chopping progress reaches 100%.
* Plays a Niagara particle effect on spawn that automatically deactivates after 3 seconds.
* Subclasses (or Blueprint instances) should set the inherited IngredientName property
* to identify themselves to ABread::CombineItems (e.g., "Tomato", "Lettuce", "Onion").
*/
UCLASS()
class SURVIVE_AND_FRY_API AChoppedVegetable : public AItem
{
	GENERATED_BODY()
public:
	AChoppedVegetable();

protected:
	virtual void BeginPlay() override;

private:
	// Particle effect played on spawn to indicate a successful chop
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* ChoppingEffect;

	FTimerHandle ChoppingTimerHandle;

	// Timer callback that deactivates the chopping particle effect after a delay
	void DisableEffects();
};
