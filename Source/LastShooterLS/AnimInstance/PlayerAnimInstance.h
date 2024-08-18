/**
 * @file PlayerAnimInstance.h
 * @brief This file contains the declaration of the UPlayerAnimInstance class.
 */

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class ABelicaCharacter;

/**
 * @class UPlayerAnimInstance
 * @brief This class represents the animation instance for the Belica character.
 *
 * It inherits from the UAnimInstance class and provides methods for handling character-specific animations.
 */
UCLASS()
class LASTSHOOTERLS_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/**
	 * @brief Called when the animation instance is initialized.
	 * This function calls the parent class's NativeInitializeAnimation function and initializes the Belica character.
	 */
	virtual void NativeInitializeAnimation() override;

	/**
	 * @brief Called every frame to update the animation instance.
	 * This function calls the parent class's NativeUpdateAnimation function and updates the animation state based on the Belica character's state.
	 * @param DeltaTime The time since the last frame.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	/**
	 * @brief The Belica character that this animation instance is associated with.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Belica", meta=(AllowPrivateAccess = "true"))
	ABelicaCharacter* Belica;

	/**
	 * @brief The movement speed of the Belica character.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	float MovementSpeed;

	/**
	 * @brief Whether the Belica character is in the air (falling or jumping).
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;

	/**
	 * @brief Whether the Belica character is accelerating.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	/**
	 * @brief Whether the Belica character is crouching.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsCrouching;

	/**
	 * @brief Whether the Belica character is aiming.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	bool bAiming;
	
	/**
	 * @brief The yaw offset of the Belica character's movement.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

protected:

};