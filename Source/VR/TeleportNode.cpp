// Fill out your copyright notice in the Description page of Project Settings.

#include "TeleportNode.h"
#include "ConstructorHelpers.h"

// Sets default values
ATeleportNode::ATeleportNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/** mesh  */
	TeleportNodeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportNodeMesh"));
	SetRootComponent(TeleportNodeMesh);

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (Sphere.Succeeded())
	{
		TeleportNodeMesh->SetStaticMesh(Sphere.Object);
	}
}
