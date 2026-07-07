#include "ChoppedVegetable.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"

AChoppedVegetable::AChoppedVegetable()
{
	PrimaryActorTick.bCanEverTick = false;

	ChoppingEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ChoppingEffect"));
	ChoppingEffect->SetupAttachment(RootSceneComponent);
}

void AChoppedVegetable::BeginPlay()
{
	Super::BeginPlay();

	// Activate the particle effect immediately on spawn and start the auto-disable timer
	if (ChoppingEffect != nullptr)
	{
		ChoppingEffect->Activate();

		// Timer that deactivates the effect 3 seconds after spawn
		GetWorldTimerManager().SetTimer(ChoppingTimerHandle, this, &AChoppedVegetable::DisableEffects, 3.f, false);
	}
}

void AChoppedVegetable::DisableEffects()
{
	if (ChoppingEffect != nullptr)
	{
		ChoppingEffect->Deactivate();
	}
}
