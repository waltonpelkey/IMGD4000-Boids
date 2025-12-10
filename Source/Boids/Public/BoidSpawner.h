// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Boid.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidSpawner.generated.h"

UCLASS()
class BOIDS_API ABoidSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetRandomVector(double Min, double Max);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	int SpawnCount = 30;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere)
	int SpawnArea = 2000;

	UPROPERTY(EditAnywhere)
	bool EnableCenterSpawn = true;
};
