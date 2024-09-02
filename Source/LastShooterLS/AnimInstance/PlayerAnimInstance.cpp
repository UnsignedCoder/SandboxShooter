/**
 * @file PlayerAnimInstance.cpp
 * @brief This file contains the implementation of the UPlayerAnimInstance class.
 */

#include "PlayerAnimInstance.h"
#include "LastShooterLS/Character/BelicaCharacter.h"
#include "KismetAnimationLibrary.h"
#include "CharacterAttributeModule/WeaponHandling/Public/WeaponHandlingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * @brief Called when the animation instance is initialized.
 * This function calls the parent class's NativeInitializeAnimation function and initializes the Belica character.
 */
void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Cast the owner of this animation instance to ABelicaCharacter
	Belica = Cast<ABelicaCharacter>(TryGetPawnOwner());
}

/**
 * @brief Called every frame to update the animation instance.
 * This function calls the parent class's NativeUpdateAnimation function and updates the animation state based on the Belica character's state.
 * @param DeltaTime The time since the last frame.
 */
void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (Belica == nullptr)
	{
		// If Belica is null, try to get the owner of this animation instance and cast it to ABelicaCharacter
		Belica = Cast<ABelicaCharacter>(TryGetPawnOwner());
	}
	if (Belica)
	{
		// Get the velocity of the Belica character and set the Z component to 0
		FVector PlayerVelocity = Belica->GetVelocity();
		PlayerVelocity.Z = 0;

		// Set the movement speed to the size of the velocity vector
		MovementSpeed = PlayerVelocity.Size();

		// Check if the Belica character is falling
		bIsInAir = Belica->GetCharacterMovement()->IsFalling();
		
		// Check if the Belica character is accelerating
		bIsAccelerating = Belica->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;

		// Check if the Belica character is crouching
		bIsCrouching = Belica->GetCharacterMovement()->IsCrouching();

		// Check if the Belica character is aiming
		bAiming = Belica->GetWeaponHandling()->GetIsAiming();
		
		// Calculate the movement offset yaw
		MovementOffsetYaw = UKismetAnimationLibrary::CalculateDirection(PlayerVelocity, Belica->GetActorRotation());
	}
}
