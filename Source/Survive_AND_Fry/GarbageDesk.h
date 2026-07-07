#pragma once

#include "CoreMinimal.h"
#include "ItemDesk.h"
#include "GarbageDesk.generated.h"

/*
* Disposal desk that automatically destroys any item placed on it.
* A repeating timer checks every 3 seconds for an item and destroys it,
* playing a sound effect on disposal.
*/
UCLASS()
class SURVIVE_AND_FRY_API AGarbageDesk : public AItemDesk
{
	GENERATED_BODY()

public:
	AGarbageDesk();

protected:
	virtual void BeginPlay() override;

private:
	// Repeating timer that checks for and destroys items on this desk
	FTimerHandle ActorRemoverTimer;

	// Timer callback
	// Destroys ItemOnDesk if present and plays the disposal sound
	void DeleteActor();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	class USoundBase* GarbageSound;
};
