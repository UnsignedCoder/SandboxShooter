/**
 * @file WeaponHandlingComponent.cpp
 * @brief This file contains the implementation of the UWeaponHandlingComponent class.
 */

#include "CharacterAttributeModule/WeaponHandling/Public/WeaponHandlingComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


/**
 * @brief Sets default values for this component's properties.
 * Initializes the component with default values for camera field of view, aiming state, and bullet spread multipliers.
 * Also sets the component to be initialized when the game starts and to be ticked every frame.
 */
UWeaponHandlingComponent::UWeaponHandlingComponent() : DefaultCameraFOV(90), CurrentCameraFOV(DefaultCameraFOV), ZoomedCameraFOV(45), ZoomInterpSpeed(20), bIsAiming(false),

//Crosshair multipliers default values for bullet spread
AcceleratingCrosshairMultiplier(0), InAirCrosshairMultiplier(0), WeaponFireWeaponCrosshairMultiplier(0), AimingCrosshairMultiplier(0),bIsFiringWeapon(false),

//Weapon fire rate
bShouldFireWeapon(true), WeaponFireRate(0.05) {
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	// You can turn these features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

/**
 * @brief Called when the game starts.
 * Calls the parent class's BeginPlay function.
 */
void UWeaponHandlingComponent::BeginPlay() { Super::BeginPlay(); }

/**
 * @brief Called every frame.
 * Calls the parent class's TickComponent function.
 * @param DeltaTime The time since the last frame.
 * @param TickType The type of tick this frame.
 * @param ThisTickFunction The tick function that caused this to run.
 */
void UWeaponHandlingComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) { Super::TickComponent(DeltaTime, TickType, ThisTickFunction); }

/**
 * @brief Traces under the crosshair.
 * Performs a line trace from the center of the screen (crosshair location) and checks if it hits anything.
 * @param TraceHitResult The result of the trace.
 * @param TraceEndLocation The end location of the trace.
 * @return True if the trace hit something, false otherwise.
 */
bool UWeaponHandlingComponent::TraceUnderCrosshair( FHitResult& TraceHitResult, FVector& TraceEndLocation ) {
	// Get the viewport size
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	// Calculate the crosshair location in the middle of the screen
	const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Convert the crosshair location from screen space to world space
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if ( bScreenToWorld ) {
		// Perform a line trace from the crosshair position into the world
		const FVector TraceStart = CrosshairWorldPosition;
		const FVector TraceEnd = TraceStart + CrosshairWorldDirection * 50000.f;
		TraceEndLocation = TraceEnd;

		// Perform the line trace
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
		if ( TraceHitResult.bBlockingHit ) {
			// If the trace hit something, update the end location and return true
			TraceEndLocation = TraceHitResult.Location;
			return true;
		}
	}
	// If the trace didn't hit anything, return false
	return false;
}

/**
 * @brief Performs a weapon trace.
 * Performs a line trace from the weapon to the location under the crosshair and checks if it hits anything.
 * @param TraceStart The start location of the trace.
 * @param TraceEnd The end location of the trace.
 * @return True if the trace hit something, false otherwise.
 */
bool UWeaponHandlingComponent::WeaponTrace( const FVector& TraceStart, FVector& TraceEnd ) {
	// Perform a trace under the crosshair
	FHitResult WeaponTraceHitResult;
	bool bCrosshairHit = TraceUnderCrosshair(WeaponTraceHitResult, TraceEnd);

	if ( bCrosshairHit ) {
		// If the crosshair trace hit something, update the end location
		TraceEnd = WeaponTraceHitResult.Location;
	}

	// Perform a second trace from the gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart = TraceStart;
	const FVector StartToEnd = TraceEnd - TraceStart;
	const FVector WeaponTraceEnd = TraceStart + StartToEnd * 1.25f;
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

	if ( WeaponTraceHit.bBlockingHit ) {
		// If the weapon trace hit something, update the end location and return true
		TraceEnd = WeaponTraceHit.Location;
		return true;
	}
	// If the weapon trace didn't hit anything, return false
	return false;
}

/**
 * @brief Changes the camera field of view based on whether the character is aiming or not.
 * @param DeltaTime The time since the last frame.
 */
void UWeaponHandlingComponent::ChangeCameraFOV( float DeltaTime ) {
	// Get the camera component
	UCameraComponent* Camera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	if ( Camera ) {
		// If the character is aiming, interpolate the camera field of view towards the zoomed field of view
		// Otherwise, interpolate it towards the default field of view
		if ( GetIsAiming() ) { CurrentCameraFOV = FMath::FInterpTo(CurrentCameraFOV, ZoomedCameraFOV, DeltaTime, ZoomInterpSpeed); }
		else { CurrentCameraFOV = FMath::FInterpTo(CurrentCameraFOV, DefaultCameraFOV, DeltaTime, ZoomInterpSpeed); }
		// Set the camera's field of view
		Camera->SetFieldOfView(CurrentCameraFOV);
	}
}

/**
 * @brief Sets the aiming state of the character.
 * @param bNewAiming The new aiming state.
 */
void UWeaponHandlingComponent::SetIsAiming( bool bNewAiming ) {
	// Set the aiming state
	bIsAiming = bNewAiming;
}

/**
 * @brief Adjusts the crosshair spread based on various factors such as player speed, whether the player is in the air, and whether the player is aiming or firing.
 * @param DeltaTime The time since the last frame.
 * @param PlayerSpeed The current speed of the player.
 * @param MaxSpeed The maximum speed of the player.
 * @param bIsInAir Whether the player is in the air.
 * @param CrosshairMultiplier The multiplier for the crosshair spread.
 */
void UWeaponHandlingComponent::DynamicCrosshair( float DeltaTime, const float PlayerSpeed, const float MaxSpeed, const bool bIsInAir, float& CrosshairMultiplier ) {
	// Calculate the crosshair spread based on player speed
	FVector2D VelocityMultiplier{0, 1};
	FVector2D MovementSpeed{0, MaxSpeed};
	AcceleratingCrosshairMultiplier = FMath::GetMappedRangeValueClamped(MovementSpeed, VelocityMultiplier, PlayerSpeed);

	// If the player is in the air, increase the crosshair spread
	if ( bIsInAir ) { InAirCrosshairMultiplier = FMath::FInterpTo(InAirCrosshairMultiplier, 3.0f, DeltaTime, 20.0f); }
	else { InAirCrosshairMultiplier = FMath::FInterpTo(InAirCrosshairMultiplier, 0.0f, DeltaTime, 5.0f); }

	// If the player is aiming, decrease the crosshair spread
	if ( bIsAiming ) { AimingCrosshairMultiplier = FMath::FInterpTo(AimingCrosshairMultiplier, -0.5f, DeltaTime, 12); }
	else { AimingCrosshairMultiplier = FMath::FInterpTo(AimingCrosshairMultiplier, 0.0f, DeltaTime, 15); }

	// If the player is firing, increase the crosshair spread
	if ( bIsFiringWeapon ) { WeaponFireWeaponCrosshairMultiplier = FMath::FInterpTo(WeaponFireWeaponCrosshairMultiplier, 0.3f, DeltaTime, 35); }
	else { WeaponFireWeaponCrosshairMultiplier = FMath::FInterpTo(WeaponFireWeaponCrosshairMultiplier, 0.0f, DeltaTime, 60); }

	// Calculate the total crosshair spread
	CrosshairMultiplier = 0.5 + AcceleratingCrosshairMultiplier + InAirCrosshairMultiplier + AimingCrosshairMultiplier + WeaponFireWeaponCrosshairMultiplier;
}

/**
 * @brief Starts the firing of the weapon.
 * Sets the firing state to true and starts a timer to stop the firing after a certain duration.
 */
void UWeaponHandlingComponent::SetWeaponFireState() {
	// Set the firing state to true
	bIsFiringWeapon = true;

	// Start a timer to stop firing after a certain duration
	GetWorld()->GetTimerManager().SetTimer(DynamicCrosshairWeaponFireTimer, this, &UWeaponHandlingComponent::ResetWeaponFireState, 0.05f);
}

/**
 * @brief Stops the firing of the weapon.
 * Sets the firing state to false.
 */
void UWeaponHandlingComponent::ResetWeaponFireState() {
	// Set the firing state to false
	bIsFiringWeapon = false;
}

/**
 * @brief Sets whether the weapon should fire or not.
 * @param bShouldFire The new firing state.
 * @return The updated firing state.
 */
bool UWeaponHandlingComponent::SetShouldFireWeapon( const bool bShouldFire ) {
	bShouldFireWeapon = bShouldFire;
	return bShouldFireWeapon;
}

/**
 * @brief Resets the weapon fire timer.
 * Sets the weapon to be ready to fire again.
 */
void UWeaponHandlingComponent::AutoFireTimerReset() { bShouldFireWeapon = true; }

/**
 * @brief Sets the fire timer for auto-firing the weapon.
 * Fires the weapon and sets a timer to allow the weapon to fire again after a delay.
 * @param BarrelSocketTransform The transform of the barrel socket.
 * @param WeaponFireTraceStart The start location of the weapon fire trace.
 * @param WeaponFireTraceEnd The end location of the weapon fire trace.
 */
void UWeaponHandlingComponent::SetFireTimer( const FTransform& BarrelSocketTransform, const FVector& WeaponFireTraceStart, FVector& WeaponFireTraceEnd ) {
	// Fire the weapon and prevent it from firing again until the timer resets
	FireWeapon(BarrelSocketTransform, WeaponFireTraceStart, WeaponFireTraceEnd);
	bShouldFireWeapon = false;

	// Set a timer to reset the weapon fire state after the fire rate delay
	GetWorld()->GetTimerManager().SetTimer(WeaponFireTimer, this, &UWeaponHandlingComponent::AutoFireTimerReset, WeaponFireRate);
}

/**
 * @brief Fires the weapon.
 * Plays the fire sound, performs a weapon trace, and spawns the muzzle flash and impact particles.
 * @param BarrelSocketTransform The transform of the barrel socket.
 * @param WeaponFireTraceStart The start location of the weapon fire trace.
 * @param WeaponFireTraceEnd The end location of the weapon fire trace.
 */
void UWeaponHandlingComponent::FireWeapon( const FTransform& BarrelSocketTransform, const FVector& WeaponFireTraceStart, FVector& WeaponFireTraceEnd ) {
	if ( bShouldFireWeapon ) {
		// Play the fire sound
		if ( FireSound ) { UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetOwner()->GetActorLocation()); }

		// Perform a weapon trace
		WeaponTrace(WeaponFireTraceStart, WeaponFireTraceEnd);

		// Spawn the muzzle flash
		if ( MuzzleFlash ) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BarrelSocketTransform); }

		// Spawn the impact particles
		if ( ImpactParticle ) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, WeaponFireTraceEnd);

			// Spawn the beam particles
			if ( BeamParticle ) {
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, BarrelSocketTransform);
				if ( Beam ) {
					// Set the target of the beam to the end location of the weapon fire trace
					Beam->SetVectorParameter("Target", WeaponFireTraceEnd);
				}
			}
		}

		// Update the weapon fire state
		SetWeaponFireState();
	}
}

