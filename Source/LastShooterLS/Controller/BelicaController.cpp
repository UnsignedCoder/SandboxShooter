/**
 * @file BelicaController.cpp
 * @brief This file contains the implementation of the ABelicaController class.
 */

#include "BelicaController.h"

#include "LastShooterLS/Character/BelicaCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterAttributeModule/WeaponHandling/Public/WeaponHandlingComponent.h"

/**
 * @brief Called when the controller possesses a pawn.
 * 
 * This function is invoked when the controller takes control of a pawn. It initializes
 * the EnhancedInputComponent, sets up the input mappings, and binds actions to their
 * corresponding handler methods.
 * 
 * @param aPawn The pawn that the controller possesses.
 */
void ABelicaController::OnPossess(APawn* aPawn)
{
    // Call the base class's OnPossess method
    Super::OnPossess(aPawn);

    // Cast the possessed pawn to ABelicaCharacter
    Belica = Cast<ABelicaCharacter>(aPawn);

    // Get the EnhancedInputComponent from the InputComponent
    EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    checkf(EnhancedInputComponent, TEXT("Enhanced Input component is valid"));

    // Add the BelicaMappingContext to the EnhancedInputLocalPlayerSubsystem
    if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) 
    {
        Subsystem->AddMappingContext(BelicaMappingContext, 0);
    }

    // Bind input actions to their respective methods
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABelicaController::Move);
    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABelicaController::HandleLookAndAiming);
    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABelicaController::Jump);

	EnhancedInputComponent->BindAction(WalkRunToggleAction, ETriggerEvent::Started, this, &ABelicaController::HandleWalk);
	EnhancedInputComponent->BindAction(WalkRunToggleAction, ETriggerEvent::Completed, this, &ABelicaController::HandleRun);

	EnhancedInputComponent->BindAction(ToggleCrouchAction, ETriggerEvent::Started, this, &ABelicaController::HandleCrouch);
	
    EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Triggered, this, &ABelicaController::HandleFireWeaponStart);
    EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &ABelicaController::HandleFireWeaponEnd);
	
    EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABelicaController::HandleAimStart);
    EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABelicaController::HandleAimEnd);

    EnhancedInputComponent->BindAction(UnequipWeaponAction, ETriggerEvent::Triggered, this, &ABelicaController::HandleUnEquipWeapon);
}

/**
 * @brief Called when the controller unpossesses a pawn.
 * 
 * This function is invoked when the controller releases control of a pawn. It calls
 * the base class's OnUnPossess method.
 */
void ABelicaController::OnUnPossess()
{
    // Call the base class's OnUnPossess method
    Super::OnUnPossess();
}

/**
 * @brief Called every frame to update the controller.
 * 
 * This function is invoked every frame. It updates the field of view based on the
 * character's weapon handling.
 * 
 * @param DeltaTime The time elapsed since the last frame.
 */
void ABelicaController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update the field of view based on the character's weapon handling
    if(Belica)
    {
        ensureMsgf(Belica->GetWeaponHandling(), TEXT("Weapon handling component is valid"));
        Belica->GetWeaponHandling()->ChangeCameraFOV(DeltaTime);
    }
}

/**
 * @brief Called when the Move action is triggered.
 * 
 * This function is invoked when the Move action is triggered. It processes the movement
 * input value to move the character forward/backward and right/left.
 * 
 * @param Value The input value for the Move action.
 */
void ABelicaController::Move(const FInputActionValue& Value)
{
    // Get the movement vector from the input value
    FVector2D MovementVector = Value.Get<FVector2D>();

    // Add movement input in the forward and right directions
    Belica->AddMovementInput(Belica->GetActorForwardVector(), MovementVector.Y);
    Belica->AddMovementInput(Belica->GetActorRightVector(), MovementVector.X);
}

/**
 * @brief Called when the Look action is triggered.
 * 
 * This function is invoked when the Look action is triggered. It processes the look
 * input value to rotate the character's view. The sensitivity of the rotation depends
 * on whether the character is aiming or not.
 * 
 * @param Value The input value for the Look action.
 */
void ABelicaController::HandleLookAndAiming(const FInputActionValue& Value)
{
    // Get the look axis value from the input value
    const FVector2D LookAxisValue = Value.Get<FVector2D>();

    // Add yaw and pitch input based on the look axis value
    if (Belica->GetWeaponHandling()->GetIsAiming())
    {
        Belica->AddControllerYawInput(LookAxisValue.X * LookSensitivityADS);
        Belica->AddControllerPitchInput(LookAxisValue.Y * LookSensitivityADS);
    }
    else
    {
        AddYawInput(LookAxisValue.X * LookSensitivityHipfire);
        AddPitchInput(LookAxisValue.Y * LookSensitivityHipfire);
    }
}

/**
 * @brief Called when the Jump action is triggered.
 * 
 * This function is invoked when the Jump action is triggered. It makes the character jump.
 */
void ABelicaController::Jump()
{
    // Make the character jump
    Belica->Jump();
}

/**
 * @brief Called when the FireWeapon action is triggered.
 * 
 * This function is invoked when the FireWeapon action is triggered. It handles starting
 * the weapon firing process, including playing the fire montage and setting the fire timer.
 */
void ABelicaController::HandleFireWeaponStart()
{
	Belica->StartFIreWeapon();
}

/**
 * @brief Called when the FireWeapon action ends.
 * 
 * This function is invoked when the FireWeapon action is completed. It updates the weapon's
 * firing status to allow for re-triggering.
 */
void ABelicaController::HandleFireWeaponEnd()
{
    Belica->EndWeaponFIre();
}

/**
 * @brief Called when the Aim action is started.
 * 
 * This function is invoked when the Aim action is started. It sets the aiming status to true.
 */
void ABelicaController::HandleAimStart()
{
	Belica->StartAiming();
}

/**
 * @brief Called when the Aim action is ended.
 * 
 * This function is invoked when the Aim action is completed. It sets the aiming status to false.
 */
void ABelicaController::HandleAimEnd()
{
	Belica->StopAiming();
}

/**
 * @brief Called when the UnEquipWeapon action is triggered.
 * 
 * This function is invoked when the UnEquipWeapon action is triggered. It unequips the character's current weapon.
 */
void ABelicaController::HandleUnEquipWeapon()
{
    Belica->UnEquipWeapon();
}

void ABelicaController::HandleRun()
{
	Belica->ToggleRun();
}

void ABelicaController::HandleWalk()
{
	Belica->ToggleWalk();
}

void ABelicaController::HandleCrouch()
{
	Belica->ToggleCrouch();
}
