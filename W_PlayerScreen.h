// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_PlayerScreen.generated.h"

/**
 * 
 */
UCLASS()
class SIDERODROMOPHOBIA_API UW_PlayerScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	/*ShowInteractionBtn
	* called from the player.
	* Shows the interaction button and also sets its text to the string passed as message.
	*/
	void ShowInteractionBtn(FString Message,class ADoorActor* DoorRef);
	/*
	* HideInteractionBtn hides the interaction button from the player
	* called from the player.
	*/
	void HideInteractionBtn();

	//called to update the stamina percentage of the player.
	void SetStaminaPercentage(float val);

	//start-general vars and functions

	/*true means set it to red and false means set it to normal.
	* Sets the player photo to the required color. Called by MainPlayer.h once the stamina is 
	* depleted.
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void EmergencyColorToPlayerPhoto(bool val);
	
	//end-general vars and functions

	UPROPERTY(meta = (BindWidget))
		class UButton* InteractionBtn;

	/*
	* Function called by DoorActor
	* This is called to switch the message, because in a case where player is overlapping with the trigger box
	* of an interactable, the message is already set. But once an interaction is done and the player still hasn't
	* left the initial overlap then, the player should get the opposite interaction type message. This is when this
	* function is called.
	*/
	void SetInteractionMessage(FString NewMessage);

private:

	//start-sprint related button and funnctions
	bool bIsLow;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* StaminaBar;
	UPROPERTY(meta = (BindWidget))
		UButton* SprintBtn;
	UFUNCTION()
		void SprintBtnPressedFunction();
	UFUNCTION()
		void SprintBtnReleasedFunction();
	UFUNCTION()
		void SetbIsLow(bool val);
	//end-sprint related button and funnctions

	//start-general vars and functions
	UPROPERTY(meta=(BindWidget))
		class UTextBlock* TxtMessage;
	UFUNCTION()
		void InteractionBtnClicked();

	//end-general vars and functions

	/*reference to the door, set by the MainPlayer body, once the player starts overlapping with
	* the door.
	*/
	ADoorActor* VarDoorRef;

	bool Initialize();
	class AMainPlayer* MainPlayerOfGame;
};
