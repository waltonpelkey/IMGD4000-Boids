// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	BoidManager = Cast<ABoidManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoidManager::StaticClass()));

	LocalFlockArea = NewObject<USphereComponent>(this, USphereComponent::StaticClass(), TEXT("Sphere"));

	if (LocalFlockArea)
	{
		LocalFlockArea->SetupAttachment(RootComponent);
		LocalFlockArea->RegisterComponent();
		LocalFlockArea->CreationMethod = EComponentCreationMethod::Instance;
		LocalFlockArea->SetSphereRadius(BoidManager->GetLocalFlockRadius());
	}
	
	Velocity = GetActorForwardVector();
	Velocity = Velocity.GetSafeNormal();
	Velocity *= FMath::RandRange(BoidManager->MinSpeed, BoidManager->MaxSpeed);
}

void ABoid::Steer(float DeltaTime)
{
	FVector Acceleration = FVector::ZeroVector;

	TArray<AActor*> LocalFlock;
	LocalFlockArea->GetOverlappingActors(LocalFlock, ABoid::StaticClass());

	Acceleration += Separate(LocalFlock);
	Acceleration += Align(LocalFlock);
	Acceleration += Cohesion(LocalFlock);

	Velocity += Acceleration * DeltaTime;
	Velocity = Velocity.GetClampedToSize(BoidManager->GetMinSpeed(), BoidManager->GetMaxSpeed());
	SetActorLocation(GetActorLocation() + (Velocity * DeltaTime));
	SetActorRotation(Velocity.ToOrientationQuat());
}

void ABoid::StayInBoundary()
{
	FVector CurrentLocation = GetActorLocation();

	if (CurrentLocation.X < -2000)
	{
		CurrentLocation.X = 2000;
	}
	else if (CurrentLocation.X > 2000)
	{
		CurrentLocation.X = -2000;
	}

	if (CurrentLocation.Y < -2000)
	{
		CurrentLocation.Y = 2000;
	}
	else if (CurrentLocation.Y > 2000)
	{
		CurrentLocation.Y = -2000;
	}

	if (CurrentLocation.Z < 0)
	{
		CurrentLocation.Z = 4000;
	}
	else if (CurrentLocation.Z > 4000)
	{
		CurrentLocation.Z = 0;
	}

	SetActorLocation(CurrentLocation);
}

FVector ABoid::Separate(TArray<AActor*> LocalFlock)
{
	FVector Steering = FVector::ZeroVector;
	FVector SeparationDirection = FVector::ZeroVector;
	int FlockCount = 0;

	for (AActor* OtherBoid : LocalFlock)
	{
		ABoid* Flockmate = Cast<ABoid>(OtherBoid);
		if (Flockmate != nullptr && Flockmate != this)
		{
			float DistanceToOtherBoid = FVector::Distance(GetActorLocation(), Flockmate->GetActorLocation());

			if (DistanceToOtherBoid > BoidManager->GetSeparationRadius())
			{
				continue;
			}

			SeparationDirection = GetActorLocation() - Flockmate->GetActorLocation();
			SeparationDirection = SeparationDirection.GetSafeNormal();
			Steering += SeparationDirection;

			FlockCount++;
		}

		
	}

	if (FlockCount > 0)
	{
		Steering /= FlockCount;
		Steering = Steering.GetSafeNormal();
		Steering -= Velocity.GetSafeNormal();
		Steering *= BoidManager->GetSeparationStrength();
		return Steering;
	}
	else
	{
		return FVector(0, 0, 0);
	}
		
}

FVector ABoid::Align(TArray<AActor*> LocalFlock)
{
	FVector Steering = FVector::ZeroVector;
	int FlockCount = 0;

	for (AActor* OtherBoid : LocalFlock)
	{
		ABoid* Flockmate = Cast<ABoid>(OtherBoid);
		if (Flockmate != nullptr && Flockmate != this)
		{
			float DistanceToOtherBoid = FVector::Distance(GetActorLocation(), Flockmate->GetActorLocation());

			if (DistanceToOtherBoid > BoidManager->GetAlignmentRadius())
			{
				continue;
			}

			Steering += Flockmate->Velocity;
			FlockCount++;
		}
	}

	if (FlockCount > 0)
	{
		Steering /= FlockCount;
		Steering = Steering.GetSafeNormal();
		Steering -= Velocity.GetSafeNormal();
		Steering *= BoidManager->GetAlignmentStrength();
		return Steering;
	}
	else
	{
		return FVector(0, 0, 0);
	}
		
}

FVector ABoid::Cohesion(TArray<AActor*> LocalFlock)
{
	FVector Steering = FVector::ZeroVector;
	FVector AveragePosition = FVector::ZeroVector;
	int FlockCount = 0;

	for (AActor* OtherBoid : LocalFlock)
	{
		ABoid* Flockmate = Cast<ABoid>(OtherBoid);
		if (Flockmate != nullptr && Flockmate != this)
		{
			float DistanceToOtherBoid = FVector::Distance(GetActorLocation(), Flockmate->GetActorLocation());

			if (DistanceToOtherBoid > BoidManager->GetCohesionRadius())
			{
				continue;
			}

			AveragePosition += Flockmate->GetActorLocation();
			FlockCount++;
		}
	}

	if (FlockCount > 0)
	{
		AveragePosition /= FlockCount;
		Steering = AveragePosition - GetActorLocation();
		Steering = Steering.GetSafeNormal();
		Steering -= Velocity.GetSafeNormal();
		Steering *= BoidManager->GetCohesionStrength();
		return Steering;
	}
	else
	{
		return FVector(0, 0, 0);
	}
		
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Steer(DeltaTime);
	StayInBoundary();
	
}

