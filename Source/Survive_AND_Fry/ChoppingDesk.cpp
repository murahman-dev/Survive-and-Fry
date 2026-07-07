#include "ChoppingDesk.h"

AChoppingDesk::AChoppingDesk()
{
	ChoppingBoard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chopping Board"));
	ChoppingBoard->SetupAttachment(DefaultSceneRoot);

	// The desk knife
	// Hidden while the player is actively chopping (player's knife shown instead)
	Knife = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Knife"));
	Knife->SetupAttachment(DefaultSceneRoot);
}
