// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"

UCLASS()
class SIDERODROMOPHOBIA_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//start-variables and functions for user user interface
	UFUNCTION(BlueprintImplementableEvent)
		class UW_PlayerScreen* CreatePlayerScreen();
	//end-variables and functions for user interface

	//start-functions regarding interactions with objects
	UFUNCTION(BlueprintCallable)
		void ShowInteraction(FString Message,class ADoorActor* DoorRef);
	UFUNCTION(BlueprintCallable)
		void HideInteraction();
	//end-functions regarding interactions with objects

	//start-functions for movement
	bool IsPlayerMoving();
	void StartSprint();
	void DoneSprint();
	//end-functions for movement

	UW_PlayerScreen* GetPlayerScreen();

private:
	//start-variables handling looking and moving for android
	//touch interface of player for in game.
	UPROPERTY(EditAnywhere)
		UTouchInterface* CustomTouchInterface;
	//touch interface of player when cutscene is activated, shouldn't have any buttons in it.
	UPROPERTY(EditAnywhere)
		UTouchInterface* NoInteractionInterface;
	UPROPERTY(EditAnywhere)
		float maxLookupVal;
	UPROPERTY(EditAnywhere)
		float minLookupVal;
	float CurrentLookupVal;

	bool bMovingForward,bMovingRight;
	bool bSprinting;
	bool bSprintReachedFull;
	float StartingSpeed;
	float SprintSpeed = 180.0f;
	float SprintTimeLeft;
	UPROPERTY(EditAnywhere)
		float SprintTime = 5.0f;
	UPROPERTY(EditAnywhere)
		float FactorForSprint = 0.8f;
	
	/*
	* Suppose the player started holding the shift key before moving, then the character will not 
	* sprint. This is correct. But now if while holding down the shift key the player started moving
	* then the player should start sprinting. So the CheckForMovement function checks this while the player holds
	* downn the sprint button.
	*/
	bool bHoldingSprint;
	void CheckForMovement();
	class UCharacterMovementComponent* varCharacterMovementComponent;
	//end-variables handling looking and moving for android

	//start-variables and functions for user interface
	UPROPERTY(EditAnywhere)
		TSubclassOf<UW_PlayerScreen> PlayerScreenClass;
	
	UW_PlayerScreen* PlayerScreen;

	//end-variables and functions for user interface

	APlayerController* PlayerControllerRef;


	//start-functions for movements and looking
	void MoveForwardBackward(float val);
	void MoveRightLeft(float val);
	
	void LookLeftRight(float val);
	void LookUpDown(float val);

	void SetbSprinting(bool val);
	void NoStaminaLeft();
	void UpdateStaminaPercentage(float val);
	//end-functions for movements and looking

	//start-functions related to hiding the player
	/*call hidetheplayer to hide the player and his menu, this is to be used only during the cutscenes.*/
	UFUNCTION(BlueprintCallable)
		void HideThePlayer();
	/*call hidetheplayer to show the player after the cutscene has been played. Otherwise don't call.*/
	UFUNCTION(BlueprintCallable)
		void ShowThePlayer();
	//end-functions related to hiding the player

};
