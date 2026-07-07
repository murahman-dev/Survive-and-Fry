#include "Bread.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayer_CC.h"
#include "NiagaraComponent.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"

// Name that identifies the antidote item
// Must match the IngredientName on the AntiDote Blueprint
static const FName AntiDoteName(TEXT("AntiDote"));

ABread::ABread()
{
	PrimaryActorTick.bCanEverTick = false;

	BreadTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BreadTop"));
	BreadTop->SetupAttachment(RootSceneComponent);

	BreadBottom = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BreadBottom"));
	BreadBottom->SetupAttachment(RootSceneComponent);

	// Ingredient mesh slots
	// Each one represents a visual layer on the sandwich
	// All set to Movable to avoid static/dynamic attachment errors at runtime
	TomatoVegetableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tomato"));
	TomatoVegetableMesh->SetupAttachment(RootSceneComponent);
	TomatoVegetableMesh->SetMobility(EComponentMobility::Movable);

	LettuceVegetableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lettuce"));
	LettuceVegetableMesh->SetupAttachment(RootSceneComponent);
	LettuceVegetableMesh->SetMobility(EComponentMobility::Movable);

	OnionVegetableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Onion"));
	OnionVegetableMesh->SetupAttachment(RootSceneComponent);
	OnionVegetableMesh->SetMobility(EComponentMobility::Movable);

	AntiDoteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AntiDote"));
	AntiDoteMesh->SetupAttachment(RootSceneComponent);

	AntiDoteEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AntiDoteEffect"));
	AntiDoteEffect->SetupAttachment(RootSceneComponent);
}

void ABread::BeginPlay()
{
	Super::BeginPlay();

	// Populate the ingredient mesh map here instead of the constructor
	// UPROPERTY TMaps get serialized into Blueprints, so if populated in the constructor,
	// old Blueprint data overrides new entries added later in C++
	IngredientMeshMap.Empty();
	IngredientMeshMap.Add(TEXT("Tomato"), TomatoVegetableMesh);
	IngredientMeshMap.Add(TEXT("Lettuce"), LettuceVegetableMesh);
	IngredientMeshMap.Add(TEXT("Onion"), OnionVegetableMesh);

	// Hide all ingredient meshes at start
	// They become visible when added via CombineItems
	for (const auto& Pair : IngredientMeshMap)
	{
		if (Pair.Value != nullptr)
		{
			Pair.Value->SetVisibility(false, true);
		}
	}

	if (AntiDoteMesh != nullptr)
	{
		AntiDoteMesh->SetVisibility(false, true);
	}
}

bool ABread::HasIngredient(FName InIngredientName) const
{
	return OwnedIngredients.Contains(InIngredientName);
}

void ABread::ConsumeItem(AItem* ItemToConsume, AMainPlayer_CC* MainPlayer)
{
	ItemToConsume->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	if (MainPlayer != nullptr)
	{
		MainPlayer->HoldingItem = nullptr;
		MainPlayer->IsHolding = false;
	}
	ItemToConsume->Destroy();
}

void ABread::CombineItems(AMainPlayer_CC* MainPlayer, AActor* SecondItem)
{
	Super::CombineItems(MainPlayer, SecondItem);

	AItem* ItemToCombine = Cast<AItem>(SecondItem);
	if (ItemToCombine == nullptr)
	{
		return;
	}

	const FName Name = ItemToCombine->IngredientName;
	if (Name.IsNone())
	{
		return;
	}

	// AntiDote has special behavior (sound + particle effect) so it's handled separately
	if (Name == AntiDoteName)
	{
		if (HasAntiDote)
		{
			UE_LOG(LogTemp, Display, TEXT("Bread Has AntiDote Already"));
			return;
		}

		HasAntiDote = true;
		ConsumeItem(ItemToCombine, MainPlayer);

		if (AntiDoteMesh != nullptr)
		{
			if (AntiDoteSound != nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), AntiDoteSound);
			}
			if (AntiDoteEffect != nullptr)
			{
				AntiDoteEffect->Activate();

				// Timer that deactivates the effect 3 seconds after it starts
				GetWorldTimerManager().SetTimer(AntiDoteTimerHandle, this, &ABread::DisableEffects, 3.f, false);
			}

			// Hide the mesh
			// The particle effect represents the antidote visually
			AntiDoteMesh->SetVisibility(false, true);
		}
		return;
	}

	// Generic ingredient handling
	// Works for any item with a recognized IngredientName
	if (OwnedIngredients.Contains(Name))
	{
		UE_LOG(LogTemp, Display, TEXT("Bread already has %s"), *Name.ToString());
		return;
	}

	OwnedIngredients.Add(Name);
	ConsumeItem(ItemToCombine, MainPlayer);

	// Show the corresponding mesh on the sandwich if one is registered
	UStaticMeshComponent** MeshPtr = IngredientMeshMap.Find(Name);
	if (MeshPtr != nullptr && *MeshPtr != nullptr)
	{
		(*MeshPtr)->SetVisibility(true, true);
	}
}

void ABread::DisableEffects()
{
	if (AntiDoteEffect != nullptr)
	{
		AntiDoteEffect->Deactivate();
	}
}
