// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeleportNode.generated.h"

UCLASS()
class VR_API ATeleportNode : public AActor
{
	GENERATED_BODY()
	
	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* TeleportNodeMesh;

public:	

	// Sets default values for this actor's properties
	ATeleportNode();
	
};
