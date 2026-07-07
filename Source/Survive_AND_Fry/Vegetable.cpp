#include "Vegetable.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayer_CC.h"
#include "Components/WidgetComponent.h"
#include "ChoppingDesk.h"

AVegetable::AVegetable()
{
	ProgressWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ProgressWidgetComponent"));
	ProgressWidgetComponent->SetupAttachment(RootSceneComponent);
}

void AVegetable::BeginPlay()
{
	Super::BeginPlay();

	// Cache a reference to the player character for updating the HUD progress bar
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerCharacter != nullptr)
	{
		MainPlayer = Cast<AMainPlayer_CC>(PlayerCharacter);
	}

	// Progress bar starts hidden
	// Shown only while actively being chopped
	if (ProgressWidgetComponent != nullptr)
	{
		ProgressWidgetComponent->SetVisibility(false);
	}
}

void AVegetable::ChopItem(float Delta)
{
	Super::ChopItem(Delta);
	if (Progress < 1.0f)
	{
		// Clamped so the progress bar never displays past 100%
		Progress = FMath::Min(Progress + Delta * ChoppingSpeed, 1.0f);

		// Show the progress bar and sync it with the player's HUD binding
		if (MainPlayer != nullptr && ProgressWidgetComponent != nullptr)
		{
			ProgressWidgetComponent->SetWorldLocation(RootSceneComponent->GetComponentLocation());
			ProgressWidgetComponent->SetVisibility(true);
			MainPlayer->Progress = Progress;
		}
		UE_LOG(LogTemp, Display, TEXT("Progres Is : %f"), Progress);
	}
	else
	{
		// Chopping complete
		// Hide the progress bar and reset the player's HUD progress value
		if (ProgressWidgetComponent != nullptr)
		{
			ProgressWidgetComponent->SetVisibility(false);
		}
		if (MainPlayer != nullptr)
		{
			MainPlayer->Progress = 0.f;
		}

		// Spawn the chopped version on the desk if SpawnChoppedActor is enabled
		AActor* ChoppingDeskPointer = GetAttachParentActor();
		if (ChoppingDeskPointer != nullptr)
		{
			AChoppingDesk* ChoppingDesk = Cast<AChoppingDesk>(ChoppingDeskPointer);
			if (ChoppingDesk != nullptr && ChoppingDesk->SpawnChoppedActor == true && ChoppedItemReference != nullptr)
			{
				AActor* ChoppedItem = GetWorld()->SpawnActor<AActor>(ChoppedItemReference, GetActorTransform());
				if (ChoppedItem != nullptr)
				{
					// Attach to the desk so it behaves like any other placed item
					ChoppedItem->AttachToComponent(ChoppingDesk->ItemLocation, FAttachmentTransformRules::KeepWorldTransform);
					ChoppingDesk->ItemOnDesk = ChoppedItem;
				}
			}
		}
		Destroy();
	}
}
