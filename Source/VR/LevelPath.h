// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelPath.generated.h"

UCLASS()
class VR_API ALevelPath : public AActor
{
	GENERATED_BODY()

	/* scene component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	
	/** spline component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USplineComponent* SplineComponent;
	
public:

	// Sets default values for this actor's properties
	ALevelPath();

	/** called to setup pawn to move reference */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetPawn(class APawn* Pawn) { PawnToMove = Pawn; }

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	/** called to move the pawn along the spline  */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void MoveCharacterAlongSpline(float DeltaTime);

	/** current point to reach at spline component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 CurrentPointIndex = 0;

	/** pawn to move along the spline  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APawn* PawnToMove;

	/** time elapsed when moving from target location to desired  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float CurrentTime = 0.f;

	/** time to wait between points */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DelayBetweenPoints = 1.f;
	

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
