/**
 * @file BelicaCharacter.h
 * @brief This file contains the declaration of the ABelicaCharacter class.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BelicaCharacter.generated.h"

class USphereComponent;
class AWeapon;
class UWeaponHandlingComponent;
class USpringArmComponent;
class UCameraComponent;
class AItem;
/**
 * @class ABelicaCharacter
 * @brief This class represents the Belica character in the game.
 *
 * It inherits from the ACharacter class and provides methods for handling character-specific actions,
 * including camera setup, weapon handling, and crosshair spread calculations.
 */
UCLASS()
class LASTSHOOTERLS_API ABelicaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/**
	 * @brief Default constructor. Sets default values for this character's properties.
	 *
	 * Initializes components such as the camera boom, follow camera, and WeaponHandling component.
	 */
	ABelicaCharacter();

protected:
	/**
	 * @brief Called when the game starts or when the character is spawned.
	 *
	 * Calls the parent class's BeginPlay function to handle initialization and
	 * then calls HandleDefaultWeaponSpawn to handle weapon setup.
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * @brief Called every frame.
	 *
	 * Updates the character by calling the parent class's Tick function and calculates
	 * the crosshair spread based on the character's movement and status.
	 * @param DeltaTime The time elapsed since the last frame.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * @brief Plays the weapon fire montage.
	 *
	 * Retrieves the character's animation instance and plays the HipFireMontage if
	 * both the animation instance and the montage are valid. Jumps to the start of the montage.
	 */
	void PlayWeaponFireMontage();

	/**
	 * @brief Calculates the crosshair spread.
	 *
	 * Determines the crosshair spread based on the character's velocity, speed, and whether
	 * the character is falling. Updates the crosshair spread using the WeaponHandling component.
	 * @param DeltaTime The time elapsed since the last frame.
	 */
	void CalculateCrosshairSpread(float DeltaTime);

	void StartAiming();
	void StopAiming();

	void StartFIreWeapon();
	void EndWeaponFIre();

	void ToggleRun();
	void ToggleWalk();

	void ToggleCrouch();
private:
	/**
	 * @brief The camera boom component that positions the follow camera behind the character.
	 *
	 * The camera boom helps keep the camera at a fixed distance behind the character while allowing
	 * it to follow the character's movements.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/**
	 * @brief The camera component that follows the character.
	 *
	 * This camera component is attached to the end of the camera boom and provides the player's view.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/**
	 * @brief The component responsible for handling weapon-related actions.
	 *
	 * This component manages weapon spawning, equipping, and other weapon-related functionality.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterAttribute, meta = (AllowPrivateAccess = "true"))
	UWeaponHandlingComponent* WeaponHandling;

	/**
	 * @brief The animation montage used for firing the weapon from the hip.
	 *
	 * This montage defines the animation sequence played when the character fires the weapon from the hip.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HipFireMontage;

	/**
	 * @brief The multiplier for the crosshair spread.
	 *
	 * This value affects the spread of the crosshair based on the character's movement and other conditions.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* EquipableItem;

public:
	/**
	 * @brief Gets the WeaponHandling component.
	 *
	 * Provides access to the WeaponHandling component for external use.
	 * @return The WeaponHandling component.
	 */
	FORCEINLINE UWeaponHandlingComponent* GetWeaponHandling() const { return WeaponHandling; }

	/**
	 * @brief Gets the crosshair spread multiplier.
	 *
	 * Provides access to the crosshair spread multiplier for external use.
	 * @return The crosshair spread multiplier.
	 */
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const { return CrosshairSpreadMultiplier; }
};
