// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldItemsModule/Item/Public/Item.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

/**
 * @brief Constructor for AItem.
 * Initializes all the item components and sets up the default item state and properties.
 * Sets collision responses and binds the overlap events for the collision sphere.
 */
AItem::AItem() : ItemCount(0), ItemRarity(EItemRarity::EIR_Common), ItemState(EItemState::EIS_InWorld),
                 // Initialize item rarity and state to default values
                 OverlappedItemCount(0), bShouldTraceForItem(false),
                 ThrowTime(4.0f), bIsFalling(false)
{
	// Enable Tick() to be called every frame for this actor.
	PrimaryActorTick.bCanEverTick = true;

	// Create the item mesh component and set it as the root component
	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// Create the collision box component used for item interaction detection
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);

	// Create the widget component for displaying item details when hovered
	ItemDetailsWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemDetailsWidget"));
	ItemDetailsWidget->SetupAttachment(GetRootComponent());

	// Create the collision sphere component used for proximity detection
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(GetRootComponent());
	CollisionSphere->SetSphereRadius(330.f); // Set the detection radius for overlap events

	// Bind the overlap events to handle player interaction
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);
}

/**
 * @brief Called when the game starts or when spawned.
 * Initializes the visibility of the item details widget and sets the active stars based on item rarity.
 * Also calls SetItemProperties() to configure the item according to its initial state.
 */
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Hide the item details widget initially
	if (ItemDetailsWidget)
	{
		ItemDetailsWidget->SetVisibility(false);
	}

	// Set the number of active stars based on the item's rarity
	SetActiveStars();

	// Set the initial properties of the item based on its state
	SetItemProperties(ItemState);
}

/**
 * @brief Called every frame.
 * Handles logic for tracing items, updating the visibility of the item details widget based on proximity and line of sight.
 * @param DeltaTime Time passed since the last frame.
 */
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Continuously trace for items if the player is in range
	HandleItemTrace();
}

/**
 * @brief Adjusts the count of overlapping players and updates the tracing flag.
 * This function is called when a player starts or stops overlapping the item's collision sphere.
 * @param Amount The amount to adjust the overlapped item count by (positive to increase, negative to decrease).
 */
void AItem::IncreaseOverlappedItemCount(int Amount)
{
	if (Amount > 0)
	{
		OverlappedItemCount += Amount;
		SetShouldTraceForItem(true);
	}
	else if (Amount <= 0)
	{
		OverlappedItemCount = 0;
		SetShouldTraceForItem(false);
	}
}

/**
 * @brief Handles item tracing logic for showing or hiding the item details widget.
 * If tracing is enabled, performs a line trace from the player's crosshair to detect items in the world.
 */
void AItem::HandleItemTrace()
{
	if (bShouldTraceForItem)
	{
		TraceForItemsInWorld();
	}
	else
	{
		ItemDetailsWidget->SetVisibility(false);
	}
}

/**
 * @brief Event handler for when an overlap begins with the item's collision sphere.
 * Increases the overlapped item count if the overlapping actor is the player character.
 * @param OverlappedComponent The component that triggered the overlap event.
 * @param OtherActor The other actor involved in the overlap.
 * @param OtherComp The other component involved in the overlap.
 * @param OtherBodyIndex The index of the other body.
 * @param bFromSweep Whether the overlap was from a sweep.
 * @param SweepResult The result of the sweep, containing hit information.
 */
void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is the player character
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);

	if (PlayerCharacter == OtherCharacter)
	{
		// Increase the overlap count to start item tracing
		IncreaseOverlappedItemCount(1);
	}
}

/**
 * @brief Event handler for when an overlap ends with the item's collision sphere.
 * Decreases the overlapped item count if the overlapping actor is the player character.
 * @param OverlappedComponent The component that ended the overlap event.
 * @param OtherActor The other actor involved in the overlap.
 * @param OtherComp The other component involved in the overlap.
 * @param OtherBodyIndex The index of the other body.
 */
void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         int32 OtherBodyIndex)
{
	// Check if the overlapping actor is the player character
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);

	if (PlayerCharacter == OtherCharacter)
	{
		// Decrease the overlap count to stop item tracing
		IncreaseOverlappedItemCount(-1);
	}
}

/**
 * @brief Traces for items in the world by performing a line trace from the crosshair position.
 * The function checks for any blocking hits and returns true if an item is hit, false otherwise.
 * @param HitItemResult The result of the line trace, containing hit information.
 * @return True if an item is hit, false otherwise.
 */
bool AItem::TraceForItems(FHitResult& HitItemResult)
{
	// Get the size of the viewport (screen size)
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	// Calculate the screen space location of the crosshair (center of the screen)
	const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Convert the screen space crosshair location to a world space direction
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
	                                                               CrosshairLocation, CrosshairWorldPosition,
	                                                               CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Define the start and end points for the line trace
		const FVector TraceStart = CrosshairWorldPosition;
		const FVector TraceEnd = TraceStart + CrosshairWorldDirection * 50000.f;
		// Large distance for long-range tracing

		// Perform the line trace and check if it hits any objects
		GetWorld()->LineTraceSingleByChannel(HitItemResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

		if (HitItemResult.bBlockingHit)
		{
			// Return true if the trace hit an item
			return true;
		}
	}
	return false; // No hit detected
}

/**
 * @brief Handles tracing for items in the world.
 * Displays the item details widget if an item is detected in the player's line of sight.
 */
void AItem::TraceForItemsInWorld()
{
	// Perform a line trace to detect items in the world
	FHitResult ItemTraceResult;
	TraceForItems(ItemTraceResult);

	if (ItemTraceResult.bBlockingHit)
	{
		// Cast the hit actor to AItem to ensure it's an item and display the details widget
		TraceItem = Cast<AItem>(ItemTraceResult.GetActor());
		if (TraceItem)
		{
			ItemDetailsWidget->SetVisibility(true);
		}
	}
	else
	{
		// Hide the item details widget if no item is hit
		ItemDetailsWidget->SetVisibility(false);
	}
}

/**
 * @brief Sets the active stars based on the item's rarity.
 * Initializes the ActiveStars array to indicate the item's rarity visually.
 */
void AItem::SetActiveStars()
{
	// Initialize all star slots to false (inactive)
	for (bool& Star : ActiveStars)
	{
		Star = false;
	}

	// Set the number of active stars based on item rarity
	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Mythic:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	default:
		break;
	}
}

/**
 * @brief Sets the item state and updates item properties accordingly.
 * This function transitions the item between different states such as being in the world, equipped, or falling.
 * @param NewState The new state to set for the item.
 */
void AItem::SetItemState(EItemState NewState)
{
	ItemState = NewState;
	SetItemProperties(ItemState); // Update item properties based on the new state
}

/**
 * @brief Sets item properties based on the current item state.
 * Configures the item mesh, collision sphere, and collision box according to the state, handling different interactions and physics properties.
 * @param State The current state of the item.
 */
void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_InWorld:
		// Set properties for the item when it is in the world
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EIS_Equipped:
		// Set properties for the item when it is equipped
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling:
		// Set properties for the item when it is falling
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	default:
		break;
	}
}

/**
 * @brief Throws the item, applying an impulse in a random direction.
 * Sets the item to a falling state and starts a timer to stop falling. The direction and intensity of the throw are randomized.
 */
void AItem::ThrowItem()
{
	// Set the rotation of the item mesh before applying impulse
	FRotator MeshRotation{0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f};
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	// Get the forward and right vectors of the item mesh for calculating the throw direction
	const FVector MeshForwardVector = GetItemMesh()->GetForwardVector();
	const FVector MeshRightVector = GetItemMesh()->GetRightVector();

	// Calculate the direction of the impulse to apply
	FVector ImpulseDirection = MeshRightVector.RotateAngleAxis(-20.0f, MeshForwardVector);
	float RandomRotation = FMath::FRandRange(-20.0f, 30.0f);

	// Apply random rotation to the impulse direction to make the throw less predictable
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.0f, 0.0f, 1.0f));
	ImpulseDirection *= 1.8f; // Scale the impulse for the throw

	// Apply the calculated impulse to the item mesh to simulate throwing
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bIsFalling = true; // Set the falling state flag

	// Start a timer to stop the item from falling after a certain duration
	GetWorldTimerManager().SetTimer(ThrowItemTimer, this, &AItem::StopFalling, ThrowTime, false);
}

/**
 * @brief Stops the item from falling and sets it back to the world state.
 * Called when the timer set in ThrowItem() expires, stopping the item's physics simulation and resetting its state.
 */
void AItem::StopFalling()
{
	bIsFalling = false; // Clear the falling state flag

	// Reset the item state to being in the world
	SetItemState(EItemState::EIS_InWorld);
}
