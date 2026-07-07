#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Bread.generated.h"

/*
* The sandwich base that the player assembles by combining chopped ingredients and an antidote.
* Ingredients are tracked via OwnedIngredients (a TSet of FName), allowing new ingredients 
* to be added without modifying this class. 
* Each ingredient name maps to a visual mesh component through IngredientMeshMap
* which is populated in BeginPlay and not the constructor to prevent Blueprint serialization 
* from overriding new entries.
* AntiDote is handled separately with its own flag, particle effect, and sound because it has
* unique visual behavior that differs from standard ingredients.
*/
UCLASS()
class SURVIVE_AND_FRY_API ABread : public AItem
{
	GENERATED_BODY()

public:
	ABread();

protected:
	virtual void BeginPlay() override;

public:
	// Identifies the incoming item by its IngredientName and either adds it as an ingredient
	// (toggling the corresponding mesh visibility) or applies it as an AntiDote
	// Uses a single AItem cast instead of per-type casts
	virtual void CombineItems(class AMainPlayer_CC* MainPlayer, class AActor* SecondItem) override;

	// Returns true if the specified ingredient has already been added to this bread
	UFUNCTION(BlueprintCallable)
	bool HasIngredient(FName InIngredientName) const;

	// Set of ingredient names currently on this bread (e.g., "Tomato", "Lettuce", "Onion")
	UPROPERTY(BlueprintReadWrite)
	TSet<FName> OwnedIngredients;

	// Tracked separately from OwnedIngredients due to unique particle and sound effects
	UPROPERTY(BlueprintReadWrite)
	bool HasAntiDote = false;

private:
	// Maps ingredient names to their corresponding mesh components for visibility toggling
	// Populated in BeginPlay rather than the constructor to avoid Blueprint serialization
	// caching stale entries when new ingredients are added in C++
	UPROPERTY()
	TMap<FName, UStaticMeshComponent*> IngredientMeshMap;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* BreadTop;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* BreadBottom;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* TomatoVegetableMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* LettuceVegetableMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* OnionVegetableMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* AntiDoteMesh;

	// Particle effect activated when the AntiDote is applied
	// Auto-disables after 3 seconds
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	class UNiagaraComponent* AntiDoteEffect;

	FTimerHandle AntiDoteTimerHandle;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	class USoundBase* AntiDoteSound;

	// Detaches the incoming item from the player's hands, clears the player's holding state,
	// and destroys the item
	// Shared by the AntiDote and generic ingredient paths
	void ConsumeItem(class AItem* ItemToConsume, class AMainPlayer_CC* MainPlayer);

	// Timer callback that deactivates the AntiDote particle effect after a delay
	void DisableEffects();
};
