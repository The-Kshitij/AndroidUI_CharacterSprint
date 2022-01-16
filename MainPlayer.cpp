/* 
* To-dos:
*Interaction functionality.
*/

#include "MainPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "W_PlayerScreen.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	//start-checking or initializing all the pointers
	PlayerControllerRef = UGameplayStatics::GetPlayerController(this,0);
	if (PlayerControllerRef==nullptr)
	{
		UE_LOG(LogTemp,Error,TEXT("PlayerControllerRef was null."));
		return;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("PlayerController found."));
	}

	if (CustomTouchInterface == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("TouchInterface was null."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TouchInterface found."));
		PlayerControllerRef->ActivateTouchInterface(CustomTouchInterface);
	}

	if (PlayerScreenClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerScreenClass was null."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerScreenClass found."));
	}

	PlayerScreen = CreatePlayerScreen();
	if (PlayerScreen == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CreatePlayerScreen returned null."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerScreen assigned."));
	}

	varCharacterMovementComponent = GetCharacterMovement();
	if (varCharacterMovementComponent==nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetCharacterMovement returned null."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterMovement assigned."));
	}
	//end-checking or initializing all the pointers

	StartingSpeed = varCharacterMovementComponent->GetMaxSpeed();
	//UE_LOG(LogTemp,Warning,TEXT("StartingSpeed is %f"),StartingSpeed);
	bSprintReachedFull = true;
	SprintTimeLeft = SprintTime;
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/*
	* The below if is when the player is sprinting, SprintTime is the max sprint time allowed.
	* Reducing the player's stamina
	*/
	if (bSprinting)
	{
		if (PlayerScreen != nullptr)
		{
			SprintTimeLeft = FMath::Clamp(SprintTimeLeft - DeltaTime * FactorForSprint, 0.0f, SprintTime);
			float val = SprintTimeLeft / SprintTime;
			//UE_LOG(LogTemp, Warning, TEXT("SprintTime is %f"), val);
			UpdateStaminaPercentage(val);

			if (FMath::IsNearlyEqual(val,0.0f))
			{
				NoStaminaLeft();
			}
		}
	}

	/*
	* The player can stop sprinting in two ways
	* 1) If the player releases the sprint btn
	* 2) If the player's stamina is depleted.
	* 
	* Either way what will happen is that the stamina should start increasing again, so the 
	* code below is ensuring that player's stamina is increasing each frame.
	* Once the stamina becomes full, the bSprintReachedFull is set to true.
	*/
	if (!bSprintReachedFull && !bSprinting)
	{
		SprintTimeLeft = FMath::Clamp(SprintTimeLeft + DeltaTime * FactorForSprint * 0.7f, 0.0f, SprintTime);
		//UE_LOG(LogTemp,Warning,TEXT("recovering sprint, SprintTimeLeft: %f"),SprintTimeLeft);
		UpdateStaminaPercentage(SprintTimeLeft/SprintTime);
		if (FMath::IsNearlyEqual(SprintTimeLeft,SprintTime))
		{
			bSprintReachedFull = true;
			UE_LOG(LogTemp, Warning, TEXT("SprintBar filled completely."));
		}
	}

	if (bHoldingSprint && !bSprinting)
	{
		CheckForMovement();
		//UE_LOG(LogTemp,Warning,TEXT("Player is holding down the sprint key."))
	}
}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"),this,&AMainPlayer::MoveForwardBackward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"),this,&AMainPlayer::MoveRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookingLeftRight"),this,&AMainPlayer::LookLeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookingUpDown"),this,&AMainPlayer::LookUpDown);

	PlayerInputComponent->BindAction(TEXT("StartSprint"),IE_Pressed,this,&AMainPlayer::StartSprint);
	PlayerInputComponent->BindAction(TEXT("StartSprint"),IE_Released,this,&AMainPlayer::DoneSprint);
}

//start-movement and looking related codes
void AMainPlayer::MoveForwardBackward(float val)
{
	if (val != 0)
	{
		//UE_LOG(LogTemp,Warning,TEXT("Moving forward * %f"),val);
		AddMovementInput(GetActorForwardVector() * val);
		bMovingForward = true;
	}
	else
	{
		bMovingForward = false;
	}
}

void AMainPlayer::MoveRightLeft(float val)
{
	if (val != 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Moving right * %f"), val);
		AddMovementInput(GetActorRightVector() * val);
		bMovingRight = true;
	}
	else
	{
		bMovingRight = false;
	}
}


void AMainPlayer::LookLeftRight(float val)
{
	if (val != 0)
	{
		AddControllerYawInput(val);
	}
}

void AMainPlayer::LookUpDown(float val)
{
	if (val != 0)
	{
		/*
		* since the player should not look up completely and rather the player should only be
		* allowed to look up or down till a certain value.
		*/
		if (CurrentLookupVal+val > minLookupVal && CurrentLookupVal+val < maxLookupVal)
		{
			AddControllerPitchInput(val);
			CurrentLookupVal += val;
			//UE_LOG(LogTemp, Warning, TEXT("LookUpValue: %f"), CurrentLookupVal);
		}
	}
}

/*
* resposible for updating the player's stamina bar
*/
void AMainPlayer::UpdateStaminaPercentage(float val)
{
	if (PlayerScreen == nullptr) return;
	
	PlayerScreen->SetStaminaPercentage(val);
}

/*
* NoStaminaLeft will be called only if the player's stamina becomes zero while sprinting. This 
* will stop player's sprinting. It is called from the if statement of the tick function.
*/
void AMainPlayer::NoStaminaLeft()
{
	SetbSprinting(false);
}

/*
* StartSprint function is called when the player presses the shift key. But since the target platform is 
* android so, it is being called from  W_PlayerScreen when the shift button is being pressed.
*/
void AMainPlayer::StartSprint()
{
	UE_LOG(LogTemp,Warning,TEXT("StartSprint is being called."));
	bHoldingSprint = true;
	if (SprintTimeLeft/SprintTime < 0.5f)
	{
		UE_LOG(LogTemp,Warning,TEXT("Can't sprint stamina is not enough."));
	}
	else if (IsPlayerMoving())
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting sprint"));
		SetbSprinting(true);
	}
}

/*
* DoneSprint function is called when the player releases the sprint button. In the case when
* the player's stamina reaches zero before releasing the sprint button, NoStaminaLeft function
* is called from the tick to stop the player from sprinting.
*/
void AMainPlayer::DoneSprint()
{
	UE_LOG(LogTemp, Warning, TEXT("Done sprint"));
	SetbSprinting(false);
	bHoldingSprint = false;
}

/*
* Just to set the bSprinting var and also perform the necessary changes with it.
*/
void AMainPlayer::SetbSprinting(bool val)
{
	bSprinting = val;
	if (bSprinting)
	{
		varCharacterMovementComponent->MaxWalkSpeed = SprintSpeed;
		bSprintReachedFull = false;
	}
	else
	{
		varCharacterMovementComponent->MaxWalkSpeed = StartingSpeed;
	}
}

/*
* IsPlayerMoving function is used by StartSprint to determine if the player is moving.
* The player should start sprinting only if the player is moving, otherwise not. So the below
* function handles this.
*/
bool AMainPlayer::IsPlayerMoving()
{
	return bMovingForward || bMovingRight;
}

void AMainPlayer::CheckForMovement()
{
	if (IsPlayerMoving())
	{
		//UE_LOG(LogTemp,Warning,TEXT("Player was holding down shift first, now started moving."));
		StartSprint();
	}
}

//end-movement and looking related codes

//start-functions for interactions
/*
* ShowInteraction function is called from blueprint of trigger box of door ref for enabling interaction of 
* player and the door.
* The DoorRef is of the overlapped door, and the Message is Close Or Open depending on the current
* state of the door.
*/
void AMainPlayer::ShowInteraction(FString Message, ADoorActor* DoorRef)
{
	if (PlayerScreen == nullptr) return;

	PlayerScreen->ShowInteractionBtn(Message,DoorRef);	
}

void AMainPlayer::HideInteraction()
{
	if (PlayerScreen == nullptr) return;

	PlayerScreen->HideInteractionBtn();
}

UW_PlayerScreen* AMainPlayer::GetPlayerScreen()
{
	return PlayerScreen;
}

//end-functions for interactions


//start-functions related to hiding the player
void AMainPlayer::HideThePlayer()
{

	if (PlayerScreen) PlayerScreen->SetVisibility(ESlateVisibility::Hidden);

	if (GetMesh()) GetMesh()->SetVisibility(false);

	if (NoInteractionInterface && PlayerControllerRef) PlayerControllerRef->ActivateTouchInterface(NoInteractionInterface);
	else
	{
		UE_LOG(LogTemp,Error,TEXT("PlayerController or nointeractioninterface is null."));
	}
}

void AMainPlayer::ShowThePlayer()
{
	if (PlayerScreen) PlayerScreen->SetVisibility(ESlateVisibility::Visible);

	if (GetMesh()) GetMesh()->SetVisibility(true);

	if (CustomTouchInterface && PlayerControllerRef) PlayerControllerRef->ActivateTouchInterface(CustomTouchInterface);

}
//end-functions related to showing the player