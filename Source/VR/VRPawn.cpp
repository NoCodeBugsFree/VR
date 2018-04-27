// Fill out your copyright notice in the Description page of Project Settings.

#include "VRPawn.h"
#include "Camera/CameraComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "ConstructorHelpers.h"
#include "VRProjectile.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/ArrowComponent.h"
#include "TeleportNode.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "InteractionInterface.h"
// #include "OculusFunctionLibrary.h" // TODO how to include ?

AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* Root capsule component  */
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(Capsule);
	Capsule->SetCapsuleHalfHeight(90.f);
	Capsule->SetCapsuleRadius(50.f);
	Capsule->SetCollisionProfileName("Pawn");

	/* vr origin scene component */
	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(RootComponent);

	/* camera component  */
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(VROrigin);
	
	/* left hand motion controller */
	MotionController_Left = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Left"));
	MotionController_Left->SetupAttachment(VROrigin);

	/* right hand motion controller */
	MotionController_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_Right"));
	MotionController_Right->SetupAttachment(VROrigin);
	MotionController_Right->Hand = EControllerHand::Right;

	/** right hand mesh component  */
	RightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHand"));
	RightHandMesh->SetupAttachment(MotionController_Right);
	RightHandMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	/** left hand mesh component  */
	LeftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHand"));
	LeftHandMesh->SetupAttachment(MotionController_Left);
	LeftHandMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (Sphere.Succeeded())
	{
		RightHandMesh->SetStaticMesh(Sphere.Object);
		LeftHandMesh->SetStaticMesh(Sphere.Object);
	}

	/**  set the projectile class */
	static ConstructorHelpers::FClassFinder<AVRProjectile> BPClass(TEXT("/Game/BP/BP_VRProjectile"));
	if (BPClass.Class)
	{
		ProjectileTemplateClass = BPClass.Class;
	}
	
	/** floating pawn movement component  */
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	FloatingPawnMovement->MaxSpeed = 300.f;

	/* arrow component  */
	LineTraceDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LineTraceDirectionArrow"));
	LineTraceDirectionArrow->SetupAttachment(LeftHandMesh);
	LineTraceDirectionArrow->ArrowSize = 10.f;

	/* sphere collision  */
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(MotionController_Right);
	InteractionSphere->SetSphereRadius(25.f);
	InteractionSphere->SetCollisionProfileName("OverlapAllDynamic");

	/** pawn defaults  */
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
}

void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AVRPawn::OnOverlapBegin);

	/** called to set tracking origin and eye(camera) location according used VR platform  */
	SetVRCamera();

	UpVector = GetActorUpVector();
}

void AVRPawn::SetVRCamera()
{
	FName HMDDeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();

	FName Oculus = FName(TEXT("Oculus Rift"));
	FName Steam = FName(TEXT("Steam VR"));
	FName PS = FName(TEXT("PSVR"));

	if (HMDDeviceName.Compare(Oculus) == 0 || HMDDeviceName.Compare(Steam) == 0)
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	else if (HMDDeviceName.Compare(PS) == 0)
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
		VROrigin->AddLocalOffset(FVector(0.f, 0.f, DefaultPlayerHeight));
	}
}

void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/** [Tick] called to handle toggle type movement  */
	// HandleToogleMovement(DeltaTime);

	/** [Tick] called to handle auto-walk movement (angle)  */
	// HandleAutoWalkMovement();

	/** [Tick] called to handle locomotion movement (shaking)  */
	// HandleLocomotionMovement(DeltaTime);

	/** [Tick] called to handle teleport movement  */
	// HandleTeleportMovement();

}

void AVRPawn::HandleTeleportMovement()
{
	if (bTeleporting)
	{
		FVector Direction = LineTraceDirectionArrow->GetForwardVector();
		FVector Start = LineTraceDirectionArrow->GetComponentLocation();
		FVector End = Start + (Direction * MaxTeleportDistance);

		FHitResult Hit;

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.bTraceComplex = false;

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 0.1f, (uint8)0, 10.f);
		if (GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams, CollisionQueryParams))
		{
			if (ATeleportNode* TeleportNodeTest = Cast<ATeleportNode>(Hit.GetActor()))
			{
				TeleportNode = TeleportNodeTest;
			}
		}
	}
}

void AVRPawn::HandleLocomotionMovement(float DeltaTime)
{
	/** create out params for GetRawSensorData  */
	FVector AngularAcceleration;
	FVector LinearAcceleration;
	FVector AngularVelocity;
	FVector LinearVelocity;
	float TimeInSeconds = 0.f;

	// UOculusFunctionLibrary::GetRawSensorData(AngularAcceleration, LinearAcceleration, AngularVelocity, LinearVelocity, TimeInSeconds, ETrackedDeviceType::HMD);

	if (FMath::Abs(LinearAcceleration.Z) >= WalkThreshold)
	{
		FVector ForwardVector = VRCamera->GetForwardVector();

		/** ignore Z-axis  */
		ForwardVector.Z = 0.f;
		AddMovementInput(ForwardVector);

		/** simulate gravity  */
		FVector CurrentLocation = GetActorLocation();
		float ZDelta = GravityScale * DeltaTime;
		FVector NewLocation = CurrentLocation - FVector(0.f, 0.f, ZDelta);
		SetActorLocation(NewLocation, true);
	}
}

void AVRPawn::HandleAutoWalkMovement()
{
	FVector ForwardVector = VRCamera->GetForwardVector();

	float Angle = (180.f) / PI * FMath::Acos(FVector::DotProduct(ForwardVector, UpVector * (-1.f)));

	/** move in camera facing direction  */
	if (Angle <= ActivateMovementAngle)
	{
		FVector ForwardVector = VRCamera->GetForwardVector();

		/** ignore Z-axis  */
		ForwardVector.Z = 0.f;
		AddMovementInput(ForwardVector);
	}
	else  /** stop  */
	{
		GetController()->StopMovement();
	}
}

void AVRPawn::HandleToogleMovement(float DeltaTime)
{
	if (bMoving) /** move in camera facing direction  */
	{
		FVector ForwardVector = VRCamera->GetForwardVector();

		/** ignore Z-axis  */
		ForwardVector.Z = 0.f;
		AddMovementInput(ForwardVector);
	}
	else /** stop  */
	{
		GetController()->StopMovement();
	}

	/** simulate gravity  */
	FVector CurrentLocation = GetActorLocation();
	float ZDelta = GravityScale * DeltaTime;
	FVector NewLocation = CurrentLocation - FVector(0.f, 0.f, ZDelta);
	SetActorLocation(NewLocation, true);
}

void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ------- Actions ---------------------------------------------------------------------

	/** interact  */
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AVRPawn::Interact);

	/** teleport  */
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AVRPawn::TeleportStart);
	PlayerInputComponent->BindAction("Teleport", IE_Released, this, &AVRPawn::TeleportEnd);

	/** fire  */
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AVRPawn::Fire);

	/** toggle movement  */
	PlayerInputComponent->BindAction("ToggleMovement", IE_Pressed, this, &AVRPawn::ToggleMovement);

	// ------- Axis ---------------------------------------------------------------------

	/** movement  */
	PlayerInputComponent->BindAxis("MoveForward", this, &AVRPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVRPawn::MoveRight);

	/** aiming  */
	PlayerInputComponent->BindAxis("Turn", this, &AVRPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AVRPawn::LookUp);
}

void AVRPawn::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AVRPawn::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AVRPawn::Turn(float Value)
{
	if (Value != 0.f)
	{
		AddControllerYawInput(Value);
	}
}

void AVRPawn::LookUp(float Value)
{
	if (Value != 0.f)
	{
		AddControllerPitchInput(Value);
	}
}

void AVRPawn::Fire()
{
	if (ProjectileTemplateClass)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator; //  GetController();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			FTransform SpawnTransform = MotionController_Right->GetComponentTransform();
			
			AVRProjectile* Spawned = World->SpawnActor<AVRProjectile>(ProjectileTemplateClass, SpawnTransform, SpawnParams);
			if (Spawned)
			{
			
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileTemplateClass == NULL"));
	}
}

void AVRPawn::TeleportStart()
{
	bTeleporting = true;
}

void AVRPawn::TeleportEnd()
{
	bTeleporting = false;

	if (TeleportNode)
	{
		SetActorLocation(TeleportNode->GetActorLocation() + FVector(0.f, 0.f, 50.f), true);
	}
}

void AVRPawn::ToggleMovement()
{
	/** flip flop the movement  */
	bMoving = !bMoving;
}

void AVRPawn::Interact()
{
	FVector Direction = LineTraceDirectionArrow->GetForwardVector();
	FVector Start = LineTraceDirectionArrow->GetComponentLocation();
	FVector End = Start + (Direction * MaxInteractionDistance);

	FHitResult Hit;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	CollisionQueryParams.bTraceComplex = false;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 1.f, (uint8)0, 10.f);
	if (GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams, CollisionQueryParams))
	{
		if (Hit.GetActor() && Hit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			if (IInteractionInterface* Item = Cast<IInteractionInterface>(Hit.GetActor()))
			{
				Item->Execute_OnInteract(Hit.GetActor());
			}
		}
	}
}

void AVRPawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself. 
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		if (IInteractionInterface* Item = Cast<IInteractionInterface>(OtherActor))
		{
			Item->Execute_OnInteract(OtherActor);
		}
	}
}
