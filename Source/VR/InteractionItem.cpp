// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionItem.h"
#include "ConstructorHelpers.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
AInteractionItem::AInteractionItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/** mesh  */
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (Sphere.Succeeded())
	{
		ItemMesh->SetStaticMesh(Sphere.Object);
	}

	/* rotating movement component */
	RotationMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotation Movement"));
	RotationMovement->RotationRate = FRotator(0.f, 180.f, 0.f);
	RotationMovement->UpdatedComponent = ItemMesh;
	RotationMovement->bAutoActivate = false;
}

void AInteractionItem::OnInteract_Implementation()
{
	RotationMovement->Activate(true);
}


