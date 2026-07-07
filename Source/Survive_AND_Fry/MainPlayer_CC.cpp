#include "MainPlayer_CC.h"
#include "Engine/World.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ItemDesk.h"
#include "Item.h"
#include "ChoppingDesk.h"
#include "ServingDesk.h"
#include "Math/UnrealMathUtility.h"
#include "Bread.h"
#include "TimerManager.h"
#include "Components/AudioComponent.h"

AMainPlayer_CC::AMainPlayer_CC()
{
	PrimaryActorTick.bCanEverTick = true;

	// Knife attached to the character's mesh
	// Shown only while chopping
	Knife = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Knife"));
	Knife->SetupAttachment(GetMesh());

	// Where held items are attached on the character
	HoldingLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Holding Location"));
	HoldingLocation->SetupAttachment(RootComponent);

	// Arrow that defines the origin and direction of the interaction trace
	PlayerTracePointer = CreateDefaultSubobject<UArrowComponent>(TEXT("TracePointer"));
	PlayerTracePointer->SetupAttachment(GetMesh());

	// Face the direction of movement instead of the controller yaw
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

void AMainPlayer_CC::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	// Hide the player's knife at start
	// Only visible during active chopping
	if (Knife != nullptr)
	{
		Knife->SetVisibility(false);
		Knife->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Knife"));
	}

	SetTaskDescription(0);

	GetWorldTimerManager().SetTimer(ChoppingHandle, this, &AMainPlayer_CC::StartChopping, ChoppingInterval, true);
}

void AMainPlayer_CC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Run the interaction trace every frame to update desk highlighting
	// and cache the desk in HighlightedDesk for the input handlers
	TraceObject();
}

void AMainPlayer_CC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainPlayer_CC::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainPlayer_CC::MoveRight);

	PlayerInputComponent->BindAction(TEXT("Grab/Release"), EInputEvent::IE_Pressed, this, &AMainPlayer_CC::GrabORRelease);
	PlayerInputComponent->BindAction(TEXT("ConfirmOrder"), EInputEvent::IE_Pressed, this, &AMainPlayer_CC::ProcessServing);
	PlayerInputComponent->BindAction(TEXT("ScaleUp"), EInputEvent::IE_Pressed, this, &AMainPlayer_CC::ScaleUp);
	PlayerInputComponent->BindAction(TEXT("ScaleDown"), EInputEvent::IE_Pressed, this, &AMainPlayer_CC::ScaleDown);
	PlayerInputComponent->BindAction(TEXT("Chop"), EInputEvent::IE_Pressed, this, &AMainPlayer_CC::ChopPressed);
	PlayerInputComponent->BindAction(TEXT("Chop"), EInputEvent::IE_Released, this, &AMainPlayer_CC::ChopReleased);
}

void AMainPlayer_CC::MoveForward(float AxisValue)
{
	AddMovementInput(FVector::ForwardVector, AxisValue);
}

void AMainPlayer_CC::MoveRight(float AxisValue)
{
	AddMovementInput(FVector::RightVector, AxisValue);
}

void AMainPlayer_CC::ChopPressed()
{
	bChopHeld = true;
}

void AMainPlayer_CC::ChopReleased()
{
	bChopHeld = false;
}

void AMainPlayer_CC::GrabORRelease()
{
	if (HighlightedDesk != nullptr && IsChopping == false)
	{
		DeskFunctions(HighlightedDesk);
	}
}

void AMainPlayer_CC::StartChopping()
{
	AChoppingDesk* Desk = Cast<AChoppingDesk>(HighlightedDesk);

	if (Desk != nullptr)
	{
		ProcessChopping(Desk);
	}
	else if (IsChopping)
	{
		StopChopping();
	}
}

void AMainPlayer_CC::TraceObject()
{
	if (PlayerTracePointer == nullptr)
	{
		return;
	}

	FVector Start = PlayerTracePointer->GetComponentLocation();
	FVector End = PlayerTracePointer->GetComponentLocation() + PlayerTracePointer->GetForwardVector() * TraceDistance;

	FHitResult Hit;
	const bool bIsHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(SphereSize));

	AItemDesk* HitDesk = bIsHit ? Cast<AItemDesk>(Hit.GetActor()) : nullptr;

	// Remove the highlight from the previous desk when the trace moves
	if (HighlightedDesk != nullptr && HighlightedDesk != HitDesk && HighlightedDesk->Desk != nullptr)
	{
		HighlightedDesk->Desk->SetRenderCustomDepth(false);
	}

	HighlightedDesk = HitDesk;

	// Enable outline highlight on the desk the player is looking at
	if (HighlightedDesk != nullptr && HighlightedDesk->Desk != nullptr)
	{
		HighlightedDesk->Desk->SetRenderCustomDepth(true);
	}
}

void AMainPlayer_CC::DeskFunctions(AItemDesk* Desk)
{
	if (Desk == nullptr)
	{
		return;
	}

	if (Desk->ItemOnDesk != nullptr)
	{
		if (HoldingItem != nullptr)
		{
			// Both player and desk have items, attempt to combine
			AItem* Item = Cast<AItem>(Desk->ItemOnDesk);
			if (Item != nullptr)
			{
				Item->CombineItems(this, HoldingItem);
			}
			UE_LOG(LogTemp, Display, TEXT("Already Equipped With An Item"));
		}
		else
		{
			// Desk has item, player is empty, pick up the item
			Desk->ItemOnDesk->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HoldingItem = Desk->ItemOnDesk;
			HoldingItem->AttachToComponent(HoldingLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			IsHolding = true;

			// If the desk auto-respawns, spawn a replacement immediately
			if (Desk->NeedToRespawn == true)
			{
				AActor* RespawnedItem = GetWorld()->SpawnActor<AActor>(Desk->ItemOnDeskReference,
					Desk->ItemLocation->GetComponentLocation(),
					Desk->ItemLocation->GetComponentRotation());

				// Attach to the desk so it behaves like any other placed item
				if (RespawnedItem != nullptr)
				{
					RespawnedItem->AttachToComponent(Desk->ItemLocation, FAttachmentTransformRules::KeepWorldTransform);
				}
				Desk->ItemOnDesk = RespawnedItem;
			}
			else
			{
				Desk->ItemOnDesk = nullptr;
			}
		}
	}
	else
	{
		if (HoldingItem != nullptr)
		{
			// Desk is empty, player has item, place it on the desk
			HoldingItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HoldingItem->AttachToComponent(Desk->ItemLocation, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Desk->ItemOnDesk = HoldingItem;

			// If placing bread on a serving desk, reset the scale and the scale display
			AServingDesk* ServingDeskReference = Cast<AServingDesk>(Desk);
			if (ServingDeskReference != nullptr)
			{
				ABread* PlacedBread = Cast<ABread>(ServingDeskReference->ItemOnDesk);
				if (PlacedBread != nullptr)
				{
					ServingDeskReference->Bread = PlacedBread;
					SetTaskDescription(1);
					ServingDeskReference->ScaleValue = 1;
					ServingDeskReference->EnlargeItem(1);
				}
			}
			HoldingItem = nullptr;
			IsHolding = false;
		}
	}
}

void AMainPlayer_CC::ProcessChopping(AChoppingDesk* Desk)
{
	if (Desk == nullptr)
	{
		return;
	}

	AItem* ItemToBeChopped = Cast<AItem>(Desk->ItemOnDesk);
	if (ItemToBeChopped == nullptr)
	{
		// Nothing left to chop (e.g., the vegetable finished and was destroyed)
		if (IsChopping)
		{
			StopChopping();
		}
		return;
	}

	// Chopping requires: the Chop input held, item is choppable, and both knives exist
	if (ItemToBeChopped->CanBeChopped == true && bChopHeld && Knife != nullptr && Desk->Knife != nullptr)
	{
		if (ChoppingSound != nullptr)
		{
			if (ChopAudioComponent == nullptr)
			{
				ChopAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), ChoppingSound, 1.f, 1.f, 0.f, nullptr, false, false);
			}
			else if (ChopAudioComponent->IsPlaying() == false)
			{
				ChopAudioComponent->Play();
			}
		}

		// Swap knife visibility
		// Hide the desk knife, show the player's
		Desk->Knife->SetVisibility(false);
		Knife->SetVisibility(true);
		IsChopping = true;
		CurrentChoppingDesk = Desk;
		
		ItemToBeChopped->ChopItem(ChoppingInterval);
	}
	else if (IsChopping)
	{
		// Not chopping, restore desk knife visibility
		StopChopping();
	}
}

void AMainPlayer_CC::StopChopping()
{
	if (Knife != nullptr)
	{
		Knife->SetVisibility(false);
	}
	if (CurrentChoppingDesk != nullptr && CurrentChoppingDesk->Knife != nullptr)
	{
		CurrentChoppingDesk->Knife->SetVisibility(true);
	}
	CurrentChoppingDesk = nullptr;
	if (ChopAudioComponent != nullptr && ChopAudioComponent->IsPlaying())
	{
		ChopAudioComponent->Stop();
	}
	IsChopping = false;
}

void AMainPlayer_CC::ProcessServing()
{
	AServingDesk* ServingDeskReference = Cast<AServingDesk>(HighlightedDesk);
	if (ServingDeskReference != nullptr)
	{
		ServingDeskReference->ServeItem();
	}
}

void AMainPlayer_CC::ScaleUp()
{
	AdjustScale(1);
}

void AMainPlayer_CC::ScaleDown()
{
	AdjustScale(-1);
}

void AMainPlayer_CC::AdjustScale(int32 Direction)
{
	AServingDesk* ServingDeskReference = Cast<AServingDesk>(HighlightedDesk);
	if (ServingDeskReference == nullptr)
	{
		return;
	}

	ABread* BreadOnDesk = Cast<ABread>(ServingDeskReference->ItemOnDesk);
	if (BreadOnDesk == nullptr)
	{
		SetTaskDescription(0);
		return;
	}

	ServingDeskReference->Bread = BreadOnDesk;
	ServingDeskReference->ScaleValue = FMath::Clamp(ServingDeskReference->ScaleValue + Direction, 1, 3);
	SetTaskDescription(ServingDeskReference->ScaleValue);
	ServingDeskReference->EnlargeItem(ServingDeskReference->ScaleValue);
}

void AMainPlayer_CC::SetTaskDescription(int32 CurrentScale)
{
	CurrentScaleDescription = CurrentScale > 0
		? FString::Printf(TEXT("CURRENT SCALE\n%dx"), CurrentScale)
		: TEXT("CURRENT SCALE\n");
}
