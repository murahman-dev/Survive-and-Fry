#include "GarbageDesk.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AGarbageDesk::AGarbageDesk()
{
}

void AGarbageDesk::BeginPlay()
{
	Super::BeginPlay();

	// Check every 3 seconds for an item to destroy
	GetWorldTimerManager().SetTimer(ActorRemoverTimer, this, &AGarbageDesk::DeleteActor, 3.f, true);
}

void AGarbageDesk::DeleteActor()
{
	if (ItemOnDesk != nullptr)
	{
		if (GarbageSound != nullptr)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), GarbageSound);
		}
		ItemOnDesk->Destroy();
		ItemOnDesk = nullptr;
	}
}
