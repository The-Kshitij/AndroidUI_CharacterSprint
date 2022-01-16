

#include "W_PlayerScreen.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "DoorActor.h"
#include "MainPlayer.h"


bool UW_PlayerScreen::Initialize()
{
	bool bSuccess=Super::Initialize();

	if (StaminaBar == nullptr) return false;

	UE_LOG(LogTemp,Warning,TEXT("StaminaBar of player found."));
	
	StaminaBar->SetPercent(1.0f);

	MainPlayerOfGame = Cast<AMainPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	if (MainPlayerOfGame)
	{
		UE_LOG(LogTemp,Warning,TEXT("MainPlayer of game found by W_PlayerScreen."));
	}
	else
	{
		UE_LOG(LogTemp,Error, TEXT("MainPlayer of game not found by W_PlayerScreen."));
		return false;
	}
	if (SprintBtn == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SprintBtn not found in W_PlayerScreen."));
		return false;
	}
	else
	{
		SprintBtn->OnPressed.AddDynamic(this, &UW_PlayerScreen::SprintBtnPressedFunction);
		SprintBtn->OnReleased.AddDynamic(this, &UW_PlayerScreen::SprintBtnReleasedFunction);
		UE_LOG(LogTemp, Warning, TEXT("SprintBtn bindings done."));
	}

	if (InteractionBtn == nullptr) return false;
	else
	{
		InteractionBtn->SetVisibility(ESlateVisibility::Hidden);
		InteractionBtn->OnClicked.AddDynamic(this,&UW_PlayerScreen::InteractionBtnClicked);
	}

	if (TxtMessage == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("TextMessage for interaction button is null"));
		return false;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("TextMessage for interaction button found."));
	}

	return bSuccess;
}

//start-Sprint related functions
void UW_PlayerScreen::SetStaminaPercentage(float val)
{
	if (StaminaBar == nullptr) return;

	StaminaBar->SetPercent(val);
	if (FMath::IsNearlyEqual(val, 0.0f))
	{
		EmergencyColorToPlayerPhoto(true);
		SetbIsLow(true);
	}
	else if (bIsLow && val > 0.5f)
	{
		EmergencyColorToPlayerPhoto(false);
		SetbIsLow(false);
	}
}

void UW_PlayerScreen::SprintBtnPressedFunction()
{
	if (MainPlayerOfGame == nullptr) return;

	if (MainPlayerOfGame->IsPlayerMoving())
	{
		MainPlayerOfGame->StartSprint();
	}
}

void UW_PlayerScreen::SprintBtnReleasedFunction()
{
	if (MainPlayerOfGame == nullptr) return;

	MainPlayerOfGame->DoneSprint();
}

void UW_PlayerScreen::SetbIsLow(bool val)
{
	bIsLow = val;
}

//end-Sprint related functions

//start-interaction related functions
void UW_PlayerScreen::InteractionBtnClicked()
{
	UE_LOG(LogTemp,Warning,TEXT("Interacting with door."));
	if (VarDoorRef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Door reference not set."));
		return;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Found door reference."));
		VarDoorRef->InteractWithDoor();
	}
}

void UW_PlayerScreen::HideInteractionBtn()
{
	//VarDoorRef = nullptr;
	if (InteractionBtn) InteractionBtn->SetVisibility(ESlateVisibility::Hidden);
}

void UW_PlayerScreen::ShowInteractionBtn(FString Message,ADoorActor* Dooref)
{
	if (InteractionBtn) InteractionBtn->SetVisibility(ESlateVisibility::Visible);

	VarDoorRef = Dooref;
	TxtMessage->SetText(FText::FromString(Message));
}

void UW_PlayerScreen::SetInteractionMessage(FString NewMessage)
{
	if (TxtMessage == nullptr) return;

	TxtMessage->SetText(FText::FromString(NewMessage));
}
//end-interaction related functions