// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VRPawn.generated.h"

UCLASS()
class VR_API AVRPawn : public APawn
{
	GENERATED_BODY()

	/* capsule component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* Capsule;

	/* vr origin scene component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* VROrigin;
	
	/* camera component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* VRCamera;

	/* left hand motion controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* MotionController_Left;

	/* right hand motion controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* MotionController_Right;

	/** right hand mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RightHandMesh;
	
	/** left hand mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LeftHandMesh;

	/** floating pawn movement component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UFloatingPawnMovement* FloatingPawnMovement;

	/* arrow component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* LineTraceDirectionArrow;
	
	/* sphere collision  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* InteractionSphere;
	
public:

	// Sets default values for this pawn's properties
	AVRPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:

	/* calls when Interaction Sphere component overlaps */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	/** called to set tracking origin and eye(camera) location according used VR platform  */
	void SetVRCamera();

	/** [Tick] called to handle toggle type movement  */
	void HandleToogleMovement(float DeltaTime);

	/** [Tick] called to handle auto-walk movement (angle)  */
	void HandleAutoWalkMovement();

	/** [Tick] called to handle teleport movement  */
	void HandleTeleportMovement();

	/** [Tick] called to handle locomotion movement (shaking)  */
	void HandleLocomotionMovement(float DeltaTime);

	/** movement  */
	void MoveForward(float Value);
	void MoveRight(float Value);

	/** aiming  */
	void Turn(float Value);
	void LookUp(float Value);

	/** called to fire a projectile  */
	void Fire();

	/** called to start attempt to teleport this pawn to teleport node actor  */
	void TeleportStart();

	/** called to stop attempt to teleport this pawn to teleport node actor  */
	void TeleportEnd();

	/** called to toggle the movement  */
	void ToggleMovement();

	/** called to interact with interactables  */
	void Interact();
	
	// -----------------------------------------------------------------------------------

	/** the height of the player  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DefaultPlayerHeight = 180.f;

	/** projectile template  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AVRProjectile> ProjectileTemplateClass;

	/** shows whether we are moving right now  or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bMoving : 1;

	/** shows whether we are trying to teleport right now  or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bTeleporting : 1;

	/** gravity scale to simulate gravity  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float GravityScale = 160.f;

	/** angle to active auto-walk movement  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ActivateMovementAngle = 70.f;
	
	UPROPERTY()
	FVector UpVector;

	/** walk threshold for locomotion movement (shaking movement)  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float WalkThreshold = 1.2f;

	/** the max amount of teleporting distance in UU  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MaxTeleportDistance = 10000.f;

	/** the max amount of interaction distance in UU  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MaxInteractionDistance = 10000.f;

	/** current teleport node to teleport when player release the teleport button  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ATeleportNode* TeleportNode;

public:	
	
};
