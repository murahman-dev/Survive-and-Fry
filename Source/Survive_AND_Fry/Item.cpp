#include "Item.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootSceneComponent);

	// All items must be Movable so they can be attached to desks and the player's holding location
	RootSceneComponent->SetMobility(EComponentMobility::Movable);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

// Base implementation with simple logging
// ABread overrides this to handle ingredient combining
void AItem::CombineItems(class AMainPlayer_CC* MainPlayer, class AActor* SecondItem)
{
	UE_LOG(LogTemp, Display, TEXT("Time To Combine Items"));
}

// Base implementation is intentionally empty
// AVegetable overrides this with chopping logic
void AItem::ChopItem(float Delta)
{

}
