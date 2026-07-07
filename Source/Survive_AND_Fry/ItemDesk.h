#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDesk.generated.h"

/*
* Base class for all workstation desks in the kitchen.
* Each desk holds a single item at a time. 
* The player can grab items from a desk, place items onto it, 
* or combine held items with the item already on the desk.
* Subclasses: AChoppingDesk, AServingDesk, AGarbageDesk.
*/
UCLASS()
class SURVIVE_AND_FRY_API AItemDesk : public AActor
{
	GENERATED_BODY()

public:
	AItemDesk();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* DefaultSceneRoot;

	// The visible desk mesh
	// Also used for the outline highlight effect via custom depth rendering
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Desk;

	// The item currently placed on this desk (null if the desk is empty)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AActor* ItemOnDesk;

	// Transform where items snap to when placed on this desk
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* ItemLocation;

	// Blueprint class used to respawn a new item when the current one is taken by the player
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TSubclassOf<AActor> ItemOnDeskReference;

	// When true, a new item is automatically spawned from ItemOnDeskReference when the player takes one
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	bool NeedToRespawn = false;
};
