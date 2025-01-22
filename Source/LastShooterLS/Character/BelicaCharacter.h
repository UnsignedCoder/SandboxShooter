/**
 * @file BelicaCharacter.h
 * @brief Defines the primary player character class with first-person shooter mechanics.
 * 
 * This character class implements core FPS gameplay systems including:
 * - Weapon management and firing mechanics
 * - Dynamic crosshair system that responds to player state
 * - Smooth camera controls with spring arm for third-person perspective
 * - Item interaction and inventory management
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
 * @brief Main player character implementing core FPS gameplay mechanics and systems.
 * 
 * This character serves as the primary player avatar, combining movement capabilities
 * from the base Character class with custom FPS mechanics. The class manages:
 * - Weapon equipping, firing, and handling through a dedicated component
 * - Dynamic crosshair system that provides visual feedback on accuracy
 * - Smooth camera transitions between first and third person views
 * - Player state management (running, walking, crouching, aiming)
 * 
 * The character uses a component-based architecture to separate concerns:
 * - WeaponHandling component manages all weapon-related functionality
 * - Spring arm and camera components handle view perspectives
 * - Sphere component manages item detection and interaction
 */
UCLASS()
class LASTSHOOTERLS_API ABelicaCharacter : public ACharacter {
	GENERATED_BODY()

public:
	/**
	 * @brief Initializes the character with required components and default settings.
	 * 
	 * Sets up the character's core components in a specific order to ensure proper initialization:
	 * 1. Camera system (spring arm + camera) for player perspective
	 * 2. Weapon handling for combat mechanics
	 * 3. Pickup sphere for item interaction
	 * 4. Default movement and input settings
	 */
	ABelicaCharacter();

protected:
	/**
	 * @brief Handles item detection when the player moves within pickup range.
	 * 
	 * Called when the pickup sphere overlaps with interactable items, allowing the
	 * character to detect and potentially interact with:
	 * - Weapons for combat
	 * - Items for inventory
	 * - Interactive world elements
	 */
	UFUNCTION()
	void OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                     const FHitResult& SweepResult );

	/**
	 * @brief Cleans up item detection when the player moves out of pickup range.
	 * 
	 * Ensures proper state management when leaving interaction zones by:
	 * - Clearing current interaction prompts
	 * - Resetting interaction states
	 * - Updating UI elements
	 */
	UFUNCTION()
	void OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

	/**
	 * @brief Initializes the character's starting loadout and systems.
	 * 
	 * Called at game start to:
	 * 1. Initialize core character systems
	 * 2. Spawn and equip default weapons
	 * 3. Set up initial player state
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * @brief Updates character systems and state each frame.
	 * 
	 * Handles continuous updates for:
	 * - Crosshair spread calculations based on movement and state
	 * - Weapon positioning and animations
	 * - Camera smoothing and transitions
	 * 
	 * @param DeltaTime Time elapsed since last frame, used for smooth interpolation
	 */
	virtual void Tick( float DeltaTime ) override;

	/**
	 * @brief Triggers weapon fire animation sequence.
	 * 
	 * Plays the appropriate firing animation based on:
	 * - Current weapon type
	 * - Character stance (hip fire vs. aimed)
	 * - Movement state
	 */
	void PlayWeaponFireMontage();

	/**
	 * @brief Updates crosshair spread based on player state and movement.
	 * 
	 * Dynamically adjusts crosshair spread to provide visual feedback on weapon accuracy.
	 * Factors considered include:
	 * - Movement speed and direction
	 * - Jumping/falling state
	 * - Aim state
	 * - Weapon type
	 * 
	 * @param DeltaTime Time since last update for smooth transitions
	 */
	void CalculateCrosshairSpread( float DeltaTime );

	/**
	 * @brief Sets up the character's initial weapon loadout.
	 * 
	 * Handles the initial weapon setup process:
	 * 1. Spawns default weapon
	 * 2. Attaches to correct socket
	 * 3. Initializes weapon state
	 */
	void HandleDefaultWeaponSpawn();

	/**
	 * @brief Manages the weapon equip process.
	 * 
	 * Coordinates the sequence of events when equipping a weapon:
	 * 1. Unequips current weapon if necessary
	 * 2. Plays equip animation
	 * 3. Updates character state
	 * 4. Initializes weapon settings
	 */
	void HandleEquipWeapon();

	/**
	 * @brief Manages the weapon unequip process.
	 * 
	 * Handles proper cleanup when unequipping weapons:
	 * 1. Plays unequip animation
	 * 2. Detaches weapon
	 * 3. Updates character state
	 * 4. Resets relevant UI elements
	 */
	void UnEquipWeapon();

	// State management functions
	void StartAiming();
	void StopAiming();

	void StartFIreWeapon();
	void EndWeaponFIre();

	void ToggleRun();
	void ToggleWalk();
	void ToggleCrouch();

private:
	/**
	 * @brief Camera positioning component for smooth follow behavior.
	 * 
	 * Uses spring arm mechanics to:
	 * - Maintain consistent camera distance
	 * - Handle collision detection
	 * - Provide smooth transition between positions
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/**
	 * @brief Main player view camera.
	 * 
	 * Provides the player's perspective and handles:
	 * - Field of view adjustments
	 * - View shake effects
	 * - Transition animations
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/**
	 * @brief Defines the area where the player can interact with items.
	 * 
	 * Used to detect when the player is close enough to:
	 * - Pick up weapons and items
	 * - Interact with world objects
	 * - Trigger environmental effects
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USphereComponent* PickupSphere;

	/**
	 * @brief Manages all weapon-related functionality.
	 * 
	 * Centralizes weapon logic to handle:
	 * - Weapon state and transitions
	 * - Ammo management
	 * - Firing mechanics
	 * - Weapon switching
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterAttribute, meta = (AllowPrivateAccess = "true"))
	UWeaponHandlingComponent* WeaponHandling;

	/**
	 * @brief Animation sequence for weapon firing.
	 * 
	 * Defines the character's firing animation, considering:
	 * - Different weapon types
	 * - Firing modes
	 * - Character stance
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HipFireMontage;

	/**
	 * @brief Current spread factor for crosshair display.
	 * 
	 * Determines visual feedback for weapon accuracy based on:
	 * - Movement state
	 * - Aim state
	 * - Weapon type
	 * - Environmental factors
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/**
	 * @brief Currently equipped weapon instance.
	 * 
	 * Tracks the active weapon to:
	 * - Manage weapon state
	 * - Handle animations
	 * - Process input
	 * - Update UI
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	/**
	 * @brief Currently focused interactable item.
	 * 
	 * Tracks items the player can interact with to:
	 * - Show interaction prompts
	 * - Handle pickup/use actions
	 * - Update UI elements
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* EquipableItem;

public:
	/**
	 * @brief Provides access to weapon handling systems.
	 * 
	 * Exposed to allow other systems to:
	 * - Query weapon state
	 * - Trigger weapon events
	 * - Modify weapon behavior
	 * 
	 * @return The character's weapon handling component
	 */
	FORCEINLINE UWeaponHandlingComponent* GetWeaponHandling() const {
		return WeaponHandling;
	}

	/**
	 * @brief Provides access to current crosshair spread value.
	 * 
	 * Exposed to allow UI and other systems to:
	 * - Update crosshair display
	 * - Modify weapon accuracy
	 * - Apply visual effects
	 * 
	 * @return Current crosshair spread multiplier
	 */
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const {
		return CrosshairSpreadMultiplier;
	}
};