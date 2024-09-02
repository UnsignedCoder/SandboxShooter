/**
 * @file Item.h
 * @brief This file contains the declaration of the AItem class.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"



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


protected:


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



public:

};
