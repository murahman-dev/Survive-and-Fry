#pragma once

#include "CoreMinimal.h"
#include "ItemDesk.h"
#include "ChoppingDesk.generated.h"

/*
* Workstation where the player chops vegetables into their chopped variants.
* Contains a knife and chopping board mesh. 
* The desk knife is hidden while the player is actively chopping 
* (the player character's knife is shown instead).
* When chopping completes, the vegetable on the desk spawns a chopped item
* if SpawnChoppedActor is enabled.
*/
UCLASS()
class SURVIVE_AND_FRY_API AChoppingDesk : public AItemDesk
{
	GENERATED_BODY()

public:
	AChoppingDesk();

public:
	// Controls whether a chopped item is spawned when chopping completes
	// Configurable per instance
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool SpawnChoppedActor = true;

	// The knife mesh on the desk
	// Visibility is toggled by the player character during chopping
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* Knife;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* ChoppingBoard;
};
