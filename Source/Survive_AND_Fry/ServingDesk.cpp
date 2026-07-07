#include "ServingDesk.h"
#include "Bread.h"
#include "IngredientData.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayer_PC.h"
#include "SNF_GameMode.h"
#include "NiagaraComponent.h"
#include "Sound/SoundBase.h"
#include "MainPlayer_CC.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"

AServingDesk::AServingDesk()
{
	ServingBoard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ServingBoard"));
	ServingBoard->SetupAttachment(DefaultSceneRoot);

	SuccessfullServeEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SuccessFullEffect"));
	SuccessfullServeEffect->SetupAttachment(DefaultSceneRoot);

	UnSuccessfullServeEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UnSuccessFullEffect"));
	UnSuccessfullServeEffect->SetupAttachment(DefaultSceneRoot);
}

void AServingDesk::BeginPlay()
{
	Super::BeginPlay();

	// Cache references to the player controller and character for score/HUD updates
	APlayerController* PlayerControllerReference = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerControllerReference != nullptr)
	{
		MainPlayer_PC = Cast<AMainPlayer_PC>(PlayerControllerReference);
	}
	ACharacter* PlayerCharacterReference = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayerCharacterReference != nullptr)
	{
		MainPlayer_CC = Cast<AMainPlayer_CC>(PlayerCharacterReference);
	}

	// The game mode owns the serve scoring rules
	GameMode = GetWorld()->GetAuthGameMode<ASNF_GameMode>();

	// Generate the first order on game start
	PickNewOrder();
}

void AServingDesk::DisableEffects()
{
	if (SuccessfullServeEffect != nullptr)
	{
		SuccessfullServeEffect->Deactivate();
	}
	if (UnSuccessfullServeEffect != nullptr)
	{
		UnSuccessfullServeEffect->Deactivate();
	}
}

void AServingDesk::PickNewOrder()
{
	if (IngredientDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ServingDesk: IngredientDataTable is not assigned!"));
		return;
	}

	TArray<FName> RowNames = IngredientDataTable->GetRowNames();
	if (RowNames.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("ServingDesk: IngredientDataTable has no rows!"));
		return;
	}

	RandomScale = FMath::RandRange(1, 3);

	const int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
	const FName& ChosenRowName = RowNames[RandomIndex];
	const FIngredientRow* Row = IngredientDataTable->FindRow<FIngredientRow>(ChosenRowName, TEXT("PickNewOrder"));

	if (Row != nullptr)
	{
		CurrentIngredientName = Row->IngredientName;
		CurrentDisplayName = Row->DisplayName;
	}

	UE_LOG(LogTemp, Display, TEXT("New Order - Scale: %d, Ingredient: %s"), RandomScale, *CurrentIngredientName.ToString());

	if (MainPlayer_PC != nullptr)
	{
		MainPlayer_PC->SetTaskDescription(RandomScale, CurrentDisplayName);
	}
}

void AServingDesk::ServeItem()
{
	Bread = Cast<ABread>(ItemOnDesk);
	if (Bread != nullptr)
	{
		if (MainPlayer_CC != nullptr)
		{
			MainPlayer_CC->SetTaskDescription(0);
		}
		ValidateServe(Bread);
	}
}

void AServingDesk::ValidateServe(ABread* BreadToServe)
{
	const bool bCorrectIngredient = BreadToServe->HasIngredient(CurrentIngredientName);
	const bool bHasAntiDote = BreadToServe->HasAntiDote;
	const bool bCorrectScale = (RandomScale == ScaleValue);
	const bool bSuccess = bCorrectIngredient && bHasAntiDote && bCorrectScale;

	if (bSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("Food Served!"));
		if (SuccessfullServeEffect != nullptr)
		{
			SuccessfullServeEffect->Activate();

			// Timer that deactivates the effect 3 seconds after the serve
			GetWorldTimerManager().SetTimer(EffectTimerHandle, this, &AServingDesk::DisableEffects, 3.f, false);
		}
		if (SuccessSound != nullptr)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), SuccessSound);
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Wrong Food Served!"));
		if (UnSuccessfullServeEffect != nullptr)
		{
			UnSuccessfullServeEffect->Activate();

			// Timer that deactivates the effect 3 seconds after the serve
			GetWorldTimerManager().SetTimer(EffectTimerHandle, this, &AServingDesk::DisableEffects, 3.f, false);
		}
		if (FailSound != nullptr)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), FailSound);
		}
	}

	// Score and timer rules live in the game mode
	if (GameMode != nullptr)
	{
		GameMode->RegisterServeResult(bSuccess);
	}

	// Common cleanup regardless of success or failure
	ItemOnDesk = nullptr;
	ScaleValue = 1;
	PickNewOrder();
	BreadToServe->Destroy();
	Bread = nullptr;
}

void AServingDesk::EnlargeItem(int32 ScaleAmount)
{
	Bread = Cast<ABread>(ItemOnDesk);
	if (Bread != nullptr)
	{
		if (ScaleAmount == 1)
		{
			Bread->SetActorScale3D(FVector(1, 1, 1));
		}
		else if (ScaleAmount == 2)
		{
			Bread->SetActorScale3D(FVector(1.5, 1.5, 1));
		}
		else
		{
			Bread->SetActorScale3D(FVector(2, 2, 1));
		}
		UE_LOG(LogTemp, Log, TEXT("Scale Is : %d"), ScaleAmount);
	}
}
