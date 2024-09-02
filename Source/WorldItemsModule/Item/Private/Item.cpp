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
AItem::AItem() {
	// Enable Tick() to be called every frame for this actor.
	PrimaryActorTick.bCanEverTick = true;

	// Create the item mesh component and set it as the root component
	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

/**
 * @brief Called when the game starts or when spawned.
 * Initializes the visibility of the item details widget and sets the active stars based on item rarity.
 * Also calls SetItemProperties() to configure the item according to its initial state.
 */
void AItem::BeginPlay() { Super::BeginPlay(); }

/**
 * @brief Called every frame.
 * Handles logic for tracing items, updating the visibility of the item details widget based on proximity and line of sight.
 * @param DeltaTime Time passed since the last frame.
 */
void AItem::Tick( float DeltaTime ) { Super::Tick(DeltaTime); }
