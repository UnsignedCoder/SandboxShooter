/**
 * @file Item.h
 * @brief This file contains the declaration of the AItem class.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

/**
 * @enum EItemRarity
 * @brief Enum to represent the rarity of an item.
 *
 * This enum defines various levels of item rarity, ranging from "Damaged" to "Default Max".
 */
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),
	EIR_Mythic UMETA(DisplayName = "Mythic"),
	EIR_Cold UMETA(DisplayName = "Cold"),
	EIR_MAX UMETA(DisplayName = "Default Max")
};

/**
 * @enum EItemState
 * @brief Enum to represent the state of an item.
 *
 * This enum defines various states an item can be in, such as being in the world, equipped, or falling.
 */
UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_InWorld UMETA(DisplayName = "InWorld"),
	EIS_Equipping UMETA(DisplayName = "Equipping"),
	EIS_Stored UMETA(DisplayName = "Stored"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
};

/**
 * @class AItem
 * @brief This class represents an item in the game world.
 *
 * It inherits from the AActor class and provides properties and methods for item interactions.
 */
UCLASS()
class WORLDITEMSMODULE_API AItem : public AActor
{
	GENERATED_BODY()

public:
	/**
	 * @brief Default constructor. Sets default values for this actor's properties.
	 *
	 * Initializes components and properties for the item actor, including collision and widget components.
	 */
	AItem();

	/**
	 * @brief Increases or decreases the count of overlapped items.
	 * @param Amount The amount to adjust the overlapped item count by.
	 *
	 * If Amount is positive, it increments the overlapped item count and sets tracing to true.
	 * If Amount is zero or negative, it resets the overlapped item count and sets tracing to false.
	 */
	void IncreaseOverlappedItemCount(int Amount);

	/**
	 * @brief Traces for items in the world.
	 *
	 * Performs a line trace from the crosshair position to detect items in the world.
	 */
	void TraceForItemsInWorld();

	/**
	 * @brief Sets whether the item should be traced.
	 * @param ShouldTrace The value to set for tracing.
	 */
	void SetShouldTraceForItem(const bool ShouldTrace) { bShouldTraceForItem = ShouldTrace; }

	/**
	 * @brief Gets whether the item should be traced.
	 * @return True if the item should be traced, false otherwise.
	 */
	bool GetShouldTraceForItem() const { return bShouldTraceForItem; }

	/**
	 * @brief Handles item tracing logic.
	 *
	 * Calls the trace function and updates the visibility of the item details widget based on tracing status.
	 */
	void HandleItemTrace();

	/**
	 * @brief Sets the active stars for the item based on its rarity.
	 *
	 * Updates the ActiveStars array to reflect the number of active stars corresponding to the item's rarity.
	 */
	void SetActiveStars();

	/**
	 * @brief Sets the properties of the item based on its state.
	 * @param ItemState The state to set the item to.
	 */
	void SetItemProperties(EItemState ItemState);

	/**
	 * @brief Throws the item, applying an impulse in a random direction.
	 *
	 * Sets the item to a falling state and starts a timer to stop falling.
	 */
	void ThrowItem();

	/**
	 * @brief Stops the item from falling and sets it back to the world state.
	 *
	 * Called when the timer set in ThrowItem() expires, stopping the item's physics simulation and resetting its state.
	 */
	void StopFalling();

	/**
	 * @brief Called when an overlap begins.
	 * @param OverlappedComponent The component that was overlapped.
	 * @param OtherActor The other actor involved in the overlap.
	 * @param OtherComp The other component involved in the overlap.
	 * @param OtherBodyIndex The index of the other body.
	 * @param bFromSweep Whether the overlap was from a sweep.
	 * @param SweepResult The result of the sweep.
	 *
	 * Increases the overlapped item count if the overlapping actor is the player character.
	 */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * @brief Called when an overlap ends.
	 * @param OverlappedComponent The component that was overlapped.
	 * @param OtherActor The other actor involved in the overlap.
	 * @param OtherComp The other component involved in the overlap.
	 * @param OtherBodyIndex The index of the other body.
	 *
	 * Decreases the overlapped item count if the overlapping actor is the player character.
	 */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);

protected:
	/**
	 * @brief Traces for items in the world.
	 * @param HitItemResult The result of the trace.
	 * @return True if the trace was successful, false otherwise.
	 *
	 * Performs a line trace from the crosshair position and returns the trace result.
	 */
	bool TraceForItems(FHitResult& HitItemResult);

	/**
	 * @brief Called when the game starts or when spawned.
	 *
	 * Initializes the item details widget and sets the active stars based on item rarity.
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * @brief Called every frame.
	 * @param DeltaTime The time since the last frame.
	 *
	 * Updates the item trace logic every frame.
	 */
	virtual void Tick(float DeltaTime) override;

private:
	/** The mesh component for the item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/** The collision box for the item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	/** The collision sphere for the item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionSphere;

	/** The widget component for item details. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ItemDetailsWidget;

	/** The item that the player is currently tracing over. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	AItem* TraceItem;

	/** The name of the item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	FText ItemName;

	/** The count of the item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	/** The rarity of the item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	/** The active stars for the item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/** The current state of the item. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	/** Timer handle for controlling the throw duration. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Property", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ThrowItemTimer;

	/** The number of items being overlapped. */
	int8 OverlappedItemCount;

	/** Whether the item should be traced. */
	bool bShouldTraceForItem;

	/** The duration for which the item should be in the thrown state. */
	float ThrowTime;

	/** Whether the item is currently falling. */
	bool bIsFalling;

public:
	/**
	 * @brief Gets the mesh component of the item.
	 * @return A pointer to the mesh component.
	 */
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	/**
	 * @brief Gets the collision box component.
	 * @return A pointer to the collision box component.
	 */
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

	/**
	 * @brief Gets the collision sphere component.
	 * @return A pointer to the collision sphere component.
	 */
	FORCEINLINE USphereComponent* GetCollisionSphere() const { return CollisionSphere; }

	/**
	 * @brief Gets the current state of the item.
	 * @return The current state of the item.
	 */
	FORCEINLINE EItemState GetItemState() const { return ItemState; }

	/**
	 * @brief Sets the state of the item.
	 * @param NewState The new state to set the item to.
	 */
	void SetItemState(EItemState NewState);

	/**
	 * @brief Gets the item currently being traced over.
	 * @return A pointer to the traced item.
	 */
	FORCEINLINE AItem* GetTraceItem() const { return TraceItem; }
};
