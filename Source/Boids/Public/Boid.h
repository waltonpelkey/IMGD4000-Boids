// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "BoidManager.h"
#include "Boid.generated.h"

UCLASS()
class BOIDS_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Steer(float DeltaTime);
	void StayInBoundary();

	ABoidManager* BoidManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* LocalFlockArea;

	FVector Separate(TArray<AActor*> LocalFlock);
	FVector Align(TArray<AActor*> LocalFlock);
	FVector Cohesion(TArray<AActor*> LocalFlock);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector Velocity;

};
