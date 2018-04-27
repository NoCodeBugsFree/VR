// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "InteractionItem.generated.h"

UCLASS()
class VR_API AInteractionItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemMesh;

	/* rotating movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class URotatingMovementComponent* RotationMovement;
	
public:

	// Sets default values for this actor's properties
	AInteractionItem();

	//~ Begin IInteractionInterface Interface
	virtual void OnInteract_Implementation();
	//~ End IInteractionInterface Interface
	
};
