/**
 * @file BelicaController.h
 * @brief This file contains the declaration of the ABelicaController class.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "BelicaController.generated.h"

class UInputAction;
class UInputMappingContext;
class ABelicaCharacter;
/**
 * @class ABelicaController
 * @brief This class is a controller for the Belica character.
 *
 * It inherits from the APlayerController class and overrides the OnPossess and OnUnPossess methods.
 * It also defines methods for handling movement, looking, jumping, and firing a weapon.
 */
UCLASS()
class LASTSHOOTERLS_API ABelicaController : public APlayerController
{
	GENERATED_BODY()

protected:
	/**
	 * @brief Called when the controller possesses a pawn.
	 * 
	 * This function is called when the controller takes control of a pawn. It sets up the
	 * EnhancedInputComponent, binds input actions to their handler methods, and applies
	 * the input mapping context.
	 * 
	 * @param aPawn The pawn that the controller possesses.
	 */
	virtual void OnPossess(APawn* aPawn) override;

	/**
	 * @brief Called when the controller unpossesses a pawn.
	 * 
	 * This function is called when the controller releases control of a pawn. It performs
	 * necessary clean-up by calling the base class's OnUnPossess method.
	 */
	virtual void OnUnPossess() override;

	/**
	 * @brief Called every frame to update the controller.
	 * 
	 * This function is called every frame. It updates the field of view based on the
	 * character's weapon handling.
	 * 
	 * @param DeltaTime The time elapsed since the last frame.
	 */
	virtual void Tick(float DeltaTime) override;	
	
public:
	/**
	 * @brief Handles the Move action.
	 * 
	 * This function is called when the Move action is triggered. It processes the movement
	 * input value to move the character forward/backward and right/left.
	 * 
	 * @param Value The input value for the Move action.
	 */
	void Move(const FInputActionValue& Value);

	/**
	 * @brief Handles the Look action.
	 * 
	 * This function is called when the Look action is triggered. It processes the look input
	 * value to rotate the character's view. The sensitivity of the rotation depends on whether
	 * the character is aiming or not.
	 * 
	 * @param Value The input value for the Look action.
	 */
	void HandleLookAndAiming(const FInputActionValue& Value);

	/**
	 * @brief Handles the Jump action.
	 * 
	 * This function is called when the Jump action is triggered. It makes the character jump.
	 */
	void Jump();

	/**
	 * @brief Handles the start of the FireWeapon action.
	 * 
	 * This function is called when the FireWeapon action is triggered. It starts the weapon
	 * firing process, including playing the fire montage and setting the fire timer.
	 */
	void HandleFireWeaponStart();

	/**
	 * @brief Handles the end of the FireWeapon action.
	 * 
	 * This function is called when the FireWeapon action is completed. It updates the weapon's
	 * firing status to allow for re-triggering.
	 */
	void HandleFireWeaponEnd();

	/**
	 * @brief Handles the start of the Aim action.
	 * 
	 * This function is called when the Aim action is started. It sets the aiming status to true.
	 */
	void HandleAimStart();

	/**
	 * @brief Handles the end of the Aim action.
	 * 
	 * This function is called when the Aim action is ended. It sets the aiming status to false.
	 */
	void HandleAimEnd();

	void HandleRun();

	void HandleWalk();

	void HandleCrouch();

private:
	/**
	 * @brief The Belica character that this controller controls.
	 * 
	 * This is a reference to the BelicaCharacter pawn that the controller is currently possessing.
	 */
	UPROPERTY()
	ABelicaCharacter* Belica;

	/**
	 * @brief The enhanced input component for the controller.
	 * 
	 * This component handles input actions and is used for binding actions to their handlers.
	 */
	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComponent;
	
	/**
	 * @brief The input mapping context for the Belica character.
	 * 
	 * This context defines the input mappings for the character's actions and is applied to
	 * the EnhancedInputLocalPlayerSubsystem.
	 */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* BelicaMappingContext;

	/**
	 * @brief The Move action.
	 * 
	 * This action represents the movement input for the character.
	 */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/**
	 * @brief The Look action.
	 * 
	 * This action represents the look/aim input for the character.
	 */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/**
	 * @brief The Jump action.
	 * 
	 * This action represents the jump input for the character.
	 */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/**
	 * @brief The FireWeapon action.
	 * 
	 * This action represents the input to fire the character's weapon.
	 */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireWeaponAction;

	/**
	 * @brief The Aim action.
	 * 
	 * This action represents the input to aim down sights with the character's weapon.
	 */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* WalkRunToggleAction;

	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ToggleCrouchAction;

	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* WeaponUnequipAction;
	
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* WeaponEquipAction;
	/**
	 * @brief The look sensitivity when the character is not aiming down sights.
	 * 
	 * This value controls the sensitivity of the character's view rotation when not aiming.
	 */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"), meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float LookSensitivityHipfire = 1.0f;

	/**
	 * @brief The look sensitivity when the character is aiming down sights.
	 * 
	 * This value controls the sensitivity of the character's view rotation when aiming down sights.
	 */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"), meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float LookSensitivityADS = 0.5f;
};
