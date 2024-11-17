/**
 * @file BelicaCharacter.cpp
 * @brief This file contains the implementation of the ABelicaCharacter class.
 */

#include "BelicaCharacter.h"

#include "Camera/CameraComponent.h"
#include "CharacterAttributeModule/WeaponHandling/Public/WeaponHandlingComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "WorldItemsModule/Item/Public/Item.h"
#include "WorldItemsModule/Weapon/Public/Weapon.h"

/**
 * @brief Sets default values for this character's properties.
 *
 * Initializes the character with a camera boom to keep the camera behind the character,
 * a follow camera, and a WeaponHandling component. Configures the character to be ticked every frame.
 */
ABelicaCharacter::ABelicaCharacter()
{
	
    // Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch =	true;
	
    // Create a camera boom (a spring arm that will keep the camera behind the character)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    // Set the camera boom properties
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
    CameraBoom->TargetArmLength = 300.f; // The distance from the player
    CameraBoom->SocketOffset = FVector(0.f, 0.f, 75.f); // Offset from the player character

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Set the character rotation to match the camera rotation
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Create a WeaponHandling component
    WeaponHandling = CreateDefaultSubobject<UWeaponHandlingComponent>(TEXT("WeaponHandling"));

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComponent);
	PickupSphere->SetSphereRadius(200.f);
	PickupSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
}


/**
 * @brief Called when the game starts or when spawned.
 *
 * Initializes the character by calling the parent class's BeginPlay function
 * and handling the default weapon spawn.
 */
void ABelicaCharacter::BeginPlay()
{
    Super::BeginPlay();

	FTimerDelegate TestTimerDelegate;
	TestTimerDelegate.BindLambda([this]()
	{
		// Handle spawning of the default weapon
		HandleDefaultWeaponSpawn();

		PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ABelicaCharacter::OnOverlapBegin);
		PickupSphere->OnComponentEndOverlap.AddDynamic(this, &ABelicaCharacter::OnOverlapEnd);
	});

	FTimerHandle TestHandle;
	GetWorldTimerManager().SetTimer(TestHandle, TestTimerDelegate, 1.f,false);

    
}

/**
 * @brief Called every frame.
 * 
 * Updates the character by calling the parent class's Tick function and calculates
 * the crosshair spread based on the character's movement and status.
 * @param DeltaTime The time since the last frame.
 */
void ABelicaCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Calculate crosshair spread based on character's velocity and movement
    CalculateCrosshairSpread(DeltaTime);

	if(WeaponHandling) {
		WeaponHandling->ChangeCameraFOV(DeltaTime);
	}
}

/**
 * @brief Plays the weapon fire montage.
 *
 * Retrieves the character's animation instance and plays the HipFireMontage if
 * both the animation instance and the montage are valid. Jumps to the start of the montage.
 */
void ABelicaCharacter::PlayWeaponFireMontage()
{
    // Get the character's animation instance
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    // If the animation instance and the HipFireMontage are both valid, play the montage
    if (AnimInstance && HipFireMontage)
    {
        AnimInstance->Montage_Play(HipFireMontage);
        AnimInstance->Montage_JumpToSection(FName("Start")); // Jump to the start of the montage
    }
}

/**
 * @brief Calculates the crosshair spread.
 *
 * Gets the player's velocity and speed, checks if the player is falling, and
 * calls the DynamicCrosshair function of the WeaponHandling component to update
 * the crosshair spread based on current conditions.
 * @param DeltaTime The time since the last frame.
 */
void ABelicaCharacter::CalculateCrosshairSpread(float DeltaTime)
{
    // Get the player's horizontal velocity
    FVector PlayerVelocity = GetCharacterMovement()->Velocity;
    PlayerVelocity.Z = 0;
    const float PlayerSpeed = PlayerVelocity.Size();

    // Get the player's maximum speed
    const float PlayerMaxSpeed = GetCharacterMovement()->GetMaxSpeed();

    // Check if the player is currently falling
    const bool bPlayerIsFalling = GetCharacterMovement()->IsFalling();

    // Update crosshair spread based on movement and falling state
    WeaponHandling->DynamicCrosshair(DeltaTime, PlayerSpeed, PlayerMaxSpeed, bPlayerIsFalling, CrosshairSpreadMultiplier);
}

/**
 * @brief Handles the default weapon spawn.
 *
 * Retrieves the socket for the weapon attachment on the character's right hand
 * and spawns the default weapon using the WeaponHandling component.
 */
void ABelicaCharacter::HandleDefaultWeaponSpawn()
{
	// Get the right-hand weapon socket
	const USkeletalMeshSocket* RightHandWeaponSocket = GetMesh()->GetSocketByName("Hand_R_Weapon_Socket");

	// Spawn the default weapon and attach it to the right-hand socket
	AWeapon* Weapon = WeaponHandling->SpawnDefaultWeapon();
	WeaponHandling->EquipWeapon(EquippedWeapon, RightHandWeaponSocket, GetMesh());
}

void ABelicaCharacter::HandleEquipWeapon()
{
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	
	if( EquipableItem ) {GEngine->AddOnScreenDebugMessage(1, 10.0f, FColor::Red, TEXT("Equipable Item") + EquipableItem->GetName());}

	// Get the right-hand weapon socket
	const USkeletalMeshSocket* RightHandWeaponSocket = GetMesh()->GetSocketByName("Hand_R_Weapon_Socket");
	if(AWeapon* EquipableWeapon = Cast<AWeapon>(EquipableItem)) {
		GEngine->AddOnScreenDebugMessage(2, 10.0f, FColor::Purple, TEXT("Equipable Weapon") + EquipableWeapon->GetName());

		WeaponHandling->EquipWeapon(EquipableWeapon, EquippedWeapon, RightHandWeaponSocket, GetMesh());

		GEngine->AddOnScreenDebugMessage(3, 10.0f, FColor::Green, TEXT("Equipped Weapon") + EquippedWeapon->GetName());
	}

	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
}

/**
 * @brief Unequips the currently equipped weapon.
 *
 * Detaches the current weapon from the character and sets its state to unarmed.
 */
void ABelicaCharacter::UnEquipWeapon()
{
	WeaponHandling->DropWeapon(EquippedWeapon);
	EquippedWeapon = nullptr;
	
	WeaponHandling->SetPlayerArmedState(EPlayerArmedState::EPAS_Unarmed);
}

void ABelicaCharacter::StartAiming()
{
	WeaponHandling->SetIsAiming(true);
}

void ABelicaCharacter::StopAiming()
{
	WeaponHandling->SetIsAiming(false);
}

void ABelicaCharacter::StartFIreWeapon()
{
	if(EquippedWeapon) {GEngine->AddOnScreenDebugMessage(5, 15.f, FColor::Red, TEXT("Equipped Weapon Name: ") + EquippedWeapon->GetName());}
	if (WeaponHandling->GetShouldFireWeapon() && WeaponHandling->GetIsArmed())
	{
		// Get the Barrel Socket from the character's mesh
		const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("SMG_Barrel");
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		// Play the weapon fire montage
		PlayWeaponFireMontage();

		// Fire the weapon
		FVector TraceEndLocation;
		WeaponHandling->SetFireTimer(SocketTransform, SocketTransform.GetLocation(), TraceEndLocation);
	}
}

void ABelicaCharacter::EndWeaponFIre()
{
	WeaponHandling->SetShouldFireWeapon(true);
}

void ABelicaCharacter::ToggleRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 900.0f;
}

void ABelicaCharacter::ToggleWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void ABelicaCharacter::ToggleCrouch()
{
	if(GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}


void ABelicaCharacter::OnOverlapBegin( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ) {
	AItem* PickupItem = Cast<AItem>(OtherActor);

	GEngine->AddOnScreenDebugMessage(1, 0.5f, FColor::Red, TEXT("Overlap Begin: ") + OtherActor->GetName());
	if(PickupItem) {
		EquipableItem = PickupItem;
	}
}
void ABelicaCharacter::OnOverlapEnd( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ) {
	EquipableItem = nullptr;
}