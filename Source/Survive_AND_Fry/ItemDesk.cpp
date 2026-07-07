#include "ItemDesk.h"
#include "Components/StaticMeshComponent.h"

AItemDesk::AItemDesk()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	// The visible desk mesh
	// Also used for highlight via custom depth rendering
	Desk = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Desk"));
	Desk->SetupAttachment(DefaultSceneRoot);

	// Items snap to this location when placed on the desk
	ItemLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Item Location"));
	ItemLocation->SetupAttachment(DefaultSceneRoot);
}

void AItemDesk::BeginPlay()
{
	Super::BeginPlay();
}
