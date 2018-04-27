// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelPath.h"
#include "Components/SplineComponent.h"

// Sets default values
ALevelPath::ALevelPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* scene component  */
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	/** spline component */
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ALevelPath::BeginPlay()
{
	Super::BeginPlay();
	
	/** set pawn reference in level BP or on actor begin overlap  */
}

void ALevelPath::MoveCharacterAlongSpline(float DeltaTime)
{
	if (PawnToMove && CurrentTime != 1.f)
	{
		/** if we do not reach the last point  */
		if (CurrentPointIndex < SplineComponent->GetNumberOfSplinePoints() - 1)
		{
			/** update current time  */
			CurrentTime = FMath::Clamp(CurrentTime + DeltaTime, 0.f, 1.f);
			
			float CurrentDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(CurrentPointIndex);
			float DesiredDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(CurrentPointIndex + 1);

			float Distance = FMath::Lerp(CurrentDistance, DesiredDistance, CurrentTime);

			FVector DesiredLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			PawnToMove->SetActorLocation(DesiredLocation, true);

			/** we reach the target point, prepare for next target point  */
			if (CurrentTime == 1.f)
			{
				/** set next point  */
				CurrentPointIndex++;

				/** reset current time by timer and start further movement  */
				FTimerDelegate NextMoveDelegate;
				NextMoveDelegate.BindLambda([&] {  CurrentTime = 0.f; });
				
				FTimerHandle NextMoveTimer;
				GetWorldTimerManager().SetTimer(NextMoveTimer, NextMoveDelegate, DelayBetweenPoints, false);
			}
		}
	}
}

// Called every frame
void ALevelPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveCharacterAlongSpline(DeltaTime);
}

