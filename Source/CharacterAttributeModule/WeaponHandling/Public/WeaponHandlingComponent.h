/**
 * @file WeaponHandlingComponent.h
 * @brief This file contains the declaration of the UWeaponHandlingComponent class.
 */

// Weapon handling component is dependent on the weapon class in the world attribute module

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponHandlingComponent.generated.h"

class AWeapon;
class USoundCue;

UENUM(BlueprintType)
enum class EPlayerArmedState : uint8
{
	EPAS_Unarmed UMETA(DisplayName = "Unarmed"),
	EPAS_Pistol UMETA(DisplayName = "Pistol"),
	EPAS_Rifle UMETA(DisplayName = "Rifle"),
	EPAS_Shotgun UMETA(DisplayName = "Shotgun"),
	EPAS_MAX UMETA(DisplayName = "DefaultMax")
};

/**
 * @class UWeaponHandlingComponent
 * @brief This class is a component for handling weapon-related actions.
 *
 * It inherits from the UActorComponent class and provides methods for firing a weapon and tracing under the crosshair.
 */
UCLASS(ClassGroup = (CharacterAttribute), meta = (BlueprintSpawnableComponent))
class CHARACTERATTRIBUTEMODULE_API UWeaponHandlingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief Default constructor. Sets default values for this component's properties.
	 */
	UWeaponHandlingComponent();

	/**
	 * @brief Fires the weapon.
	 * Plays the fire sound, performs a weapon trace, and spawns the muzzle flash and impact particles.
	 * @param BarrelSocketTransform The transform of the barrel socket.
	 * @param WeaponFireTraceStart The start location of the weapon fire trace.
	 * @param WeaponFireTraceEnd The end location of the weapon fire trace.
	 */
	void FireWeapon(const FTransform& BarrelSocketTransform, const FVector& WeaponFireTraceStart, FVector& WeaponFireTraceEnd);

	/**
	 * @brief Traces under the crosshair.
	 * Performs a line trace from the center of the screen and checks if it hits anything.
	 * @param TraceHitResult The result of the trace.
	 * @param TraceEndLocation The end location of the trace.
	 * @return True if the trace hit something, false otherwise.
	 */
	bool TraceUnderCrosshair(FHitResult& TraceHitResult, FVector& TraceEndLocation);

	/**
	 * @brief Performs a weapon trace.
	 * Performs a line trace from the weapon to the location under the crosshair and checks if it hits anything.
	 * @param TraceStart The start location of the trace.
	 * @param TraceEnd The end location of the trace.
	 * @param TraceHitResult
	 * @return True if the trace hit something, false otherwise.
	 */
	bool WeaponTrace( const FVector& TraceStart, FVector& TraceEnd, FHitResult& TraceHitResult );

	/**
	 * @brief Changes the camera field of view based on aiming state.
	 * Interpolates the camera's field of view towards the zoomed or default value based on aiming.
	 * @param DeltaTime The time since the last frame.
	 */
	void ChangeCameraFOV(float DeltaTime);

	/**
	 * @brief Sets the aiming state of the character.
	 * @param bNewAiming The new aiming state.
	 */
	void SetIsAiming(bool bNewAiming);

	/**
	 * @brief Adjusts the crosshair spread based on various factors.
	 * Calculates the crosshair spread based on player speed, whether in the air, and whether aiming or firing.
	 * @param DeltaTime The time since the last frame.
	 * @param PlayerSpeed The current speed of the player.
	 * @param MaxSpeed The maximum speed of the player.
	 * @param bIsInAir Whether the player is in the air. 
	 * @param CrosshairMultiplier The multiplier for the crosshair spread.
	 */
	void DynamicCrosshair(float DeltaTime, const float PlayerSpeed, const float MaxSpeed, const bool bIsInAir, float& CrosshairMultiplier);

	/**
	 * @brief Starts the firing of the weapon.
	 * Sets the firing state to true and starts a timer to stop the firing after a certain duration.
	 */
	void SetWeaponFireState();

	/**
	 * @brief Stops the firing of the weapon.
	 * Sets the firing state to false.
	 */
	void ResetWeaponFireState();

	/**
	 * @brief Resets the auto-fire timer.
	 * Resets the timer used for auto-firing the weapon.
	 */
	void AutoFireTimerReset();

	/**
	 * @brief Sets whether the weapon should fire or not.
	 * @param bShouldFire The new firing state.
	 * @return True if the firing state was successfully set, false otherwise.
	 */
	bool SetShouldFireWeapon(bool bShouldFire);

	/**
	 * @brief Sets a timer for the weapon fire.
	 * Sets a timer for the weapon fire with the provided parameters.
	 * @param BarrelSocketTransform The transform of the barrel socket.
	 * @param WeaponFireTraceStart The start location of the weapon fire trace.
	 * @param WeaponFireTraceEnd The end location of the weapon fire trace.
	 */
	void SetFireTimer(const FTransform& BarrelSocketTransform, const FVector& WeaponFireTraceStart, FVector& WeaponFireTraceEnd);

	/**
	 * @brief Spawns the default weapon for the character.
	 * Spawns the default weapon specified by DefaultWeaponClass and attaches it to the right-hand weapon socket.
	 * @return The spawned weapon actor.
	 */
	AWeapon* SpawnDefaultWeapon() const;

	/**
	 * @brief Equips the specified weapon.
	 * Attaches the weapon to the given weapon socket on the player's skeletal mesh.
	 * @param WeaponToEquip The weapon to be equipped.
	 * @param EquippedWeapon A reference to the equipped weapon.
	 * @param WeaponSlotSocket The socket to attach the weapon to.
	 * @param PlayerMesh The skeletal mesh component of the player.
	 */
	void EquipWeapon(AWeapon* WeaponToEquip, AWeapon*& EquippedWeapon, const USkeletalMeshSocket* WeaponSlotSocket, USkeletalMeshComponent* PlayerMesh);

	/**
	 * @brief Drops the equipped weapon.
	 * Detaches the weapon from the player's skeletal mesh and sets its state to falling.
	 * @param WeaponToDrop A reference to the weapon to be dropped.
	 */
	void DropWeapon(AWeapon*& WeaponToDrop);

	void SetPlayerArmedState(EPlayerArmedState NewPlayerArmedState);

protected:
	/**
	 * @brief Called when the game starts.
	 * Calls the parent class's BeginPlay function.
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Called every frame.
	 * Calls the parent class's TickComponent function.
	 * @param DeltaTime The time since the last frame.
	 * @param TickType The type of tick this frame.
	 * @param ThisTickFunction The tick function that caused this to run.
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	//Weapon VFX
	/** The sound to play when the weapon is fired. */
	UPROPERTY(EditAnywhere, Category = "Weapon|Sound", meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound = nullptr;

	/** The particle system for the muzzle flash. */
	UPROPERTY(EditAnywhere, Category = "Weapon|WeaponVfx", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash = nullptr;

	/** The particle system for the beam. */
	UPROPERTY(EditAnywhere, Category = "Weapon|WeaponVfx", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle = nullptr;

	/** The particle system for the impact. */
	UPROPERTY(EditAnywhere, Category = "Weapon|WeaponVfx", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle = nullptr;

	/** The class of the default weapon to be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

//Aiming related variables 
private:
	/** The default camera field of view. */
	UPROPERTY(EditAnywhere, Category = "Field of View", meta = (AllowPrivateAccess = "true"))
	float DefaultCameraFOV;

	/** The current camera field of view. */
	UPROPERTY(EditAnywhere, Category = "Field of View", meta = (AllowPrivateAccess = "true"))
	float CurrentCameraFOV;

	/** The zoomed camera field of view. */
	UPROPERTY(EditAnywhere, Category = "Field of View", meta = (AllowPrivateAccess = "true"))
	float ZoomedCameraFOV;

	/** The interpolation speed for zooming. */
	UPROPERTY(EditAnywhere, Category = "Field of View", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	/** True if the character is aiming, false otherwise. */
	UPROPERTY(VisibleAnywhere, Category = "Field of View", meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

//Dynamic Crosshair Variables
private:
	/** The crosshair spread multiplier based on player speed. */
	UPROPERTY(VisibleAnywhere, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float AcceleratingCrosshairMultiplier;

	/** The crosshair spread multiplier when the player is in the air. */
	UPROPERTY(VisibleAnywhere, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float InAirCrosshairMultiplier;

	/** The crosshair spread multiplier when the weapon is firing. */
	UPROPERTY(VisibleAnywhere, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float WeaponFireWeaponCrosshairMultiplier;

	/** The crosshair spread multiplier when the player is aiming. */
	UPROPERTY(VisibleAnywhere, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float AimingCrosshairMultiplier;

	/** True if the weapon is firing, false otherwise. */
	UPROPERTY(VisibleAnywhere, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	bool bIsFiringWeapon;

	/** The timer handle for stopping the firing of the weapon. */
	UPROPERTY(VisibleAnywhere, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	FTimerHandle DynamicCrosshairWeaponFireTimer;

//Firing weapon Variables
private:
	/** The timer handle for auto firing the weapon. */
	UPROPERTY()
	FTimerHandle WeaponFireTimer;

	/** True if the weapon should fire, false otherwise. */
	UPROPERTY(VisibleAnywhere, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	bool bShouldFireWeapon;

	/** The rate at which the weapon fires. */
	UPROPERTY(EditAnywhere, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	float WeaponFireRate;

//Weapon Armed State
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PlayerArmedState", meta = (AllowPrivateAccess = true))
	bool bIsArmed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PlayerArmedState", meta = (AllowPrivateAccess = true))
	bool bIsArmedPistol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PlayerArmedState", meta = (AllowPrivateAccess = true))
	bool bIsArmedRifle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PlayerArmedState", meta = (AllowPrivateAccess = true))
	bool bIsArmedShotGun;

public:
	/**
	 * @brief Gets the aiming state of the character.
	 * @return True if the character is aiming, false otherwise.
	 */
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

	/**
	 * @brief Gets the firing state of the weapon.
	 * @return True if the weapon should fire, false otherwise.
	 */
	FORCEINLINE bool GetShouldFireWeapon() const { return bShouldFireWeapon; }

	FORCEINLINE bool GetIsArmed() const { return bIsArmed; }

	FORCEINLINE bool GetIsArmedPistol() const { return bIsArmedPistol; }

	FORCEINLINE bool GetIsArmedRifle() const { return bIsArmedRifle; }

	FORCEINLINE bool GetIsArmedShotGun() const { return bIsArmedShotGun; }
};
