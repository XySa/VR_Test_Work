// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TurretAnimInterface.h"

#define OUT
// Sets default values
ATurret::ATurret() {
	// Set this pawn to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	TurretRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(TurretRootComponent);

	TurretMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretMeshComponent"));
	TurretMeshComponent->SetupAttachment(TurretRootComponent);

	Beam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam"));
	Beam->SetupAttachment(TurretMeshComponent,TEXT("BeamSocket"));

	Target1 = CreateDefaultSubobject<USceneComponent>(TEXT("Target1"));
	Target1->SetupAttachment(TurretRootComponent);

	Target2 = CreateDefaultSubobject<USceneComponent>(TEXT("Target2"));
	Target2->SetupAttachment(TurretRootComponent);

	FollowTarget = CreateDefaultSubobject<USceneComponent>(TEXT("FollowTarget"));
	FollowTarget->SetupAttachment(TurretRootComponent);

	SetBeamLenght(BeamLenght);

	// Set default rotation speed
	RotationSpeed = 10.0f;

	// Set default fire rate
	FireRate = 1.0f;

	// Set default detection range
	DetectionRange = 1000.0f;

	// Initialize time since last shot
	TimeSinceLastShot = 0.0f;
}

// Called when the game starts or when spawned
void ATurret::BeginPlay() {
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(ChangeTargetTimerHandle, this, &ATurret::ChangeBeamTarget, ChangeTargetDelay, true, 1.f);
	GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &ATurret::BeamTrace, .1f, true, .1f);
}

// Called every frame
void ATurret::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	UpdateLookTarget(DeltaTime);

	// Update the time since the turret last fired
	TimeSinceLastShot += DeltaTime;

	// Find the closest character within the detection range
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), FoundActors);

	float ClosestDistance = DetectionRange;
	for (AActor* Actor : FoundActors) {
		float Distance = (Actor->GetActorLocation() - GetActorLocation()).Size();
		if (Distance < ClosestDistance) {
			ClosestDistance = Distance;
			OutTarget = Actor;
		}
	}

	// If a target is found, rotate towards it and fire if possible
	if (OutTarget != nullptr) {
		RotateTowardsTarget(OutTarget);

		if (TimeSinceLastShot >= FireRate) {
			Fire();
			TimeSinceLastShot = 0.0f;
		}
	}
}

void ATurret::UpdateLookTarget(float DeltaTime) {
	if (LookAtRotation.Equals(TargetRotation, 1.f)) {
		return;
	}

	LookAtRotation += RotationDelta * RotationRateMultiplayer * DeltaTime;

	if (TurretMeshComponent->GetAnimInstance()->Implements<UTurretAnimInterface>()) {
		ITurretAnimInterface::Execute_UpdateLookAtRotation(TurretMeshComponent->GetAnimInstance(), LookAtRotation);
	}
}

void ATurret::ChangeBeamTarget() {
	TimerCount++;
	if (TimerCount % 2 == 0) {
		FollowTarget->SetWorldLocation(Target1->GetComponentLocation());
	} else {
		FollowTarget->SetWorldLocation(Target2->GetComponentLocation());
	}

	FVector Start = TurretMeshComponent->GetSocketLocation("BeamSocket");
	FVector End = FollowTarget->GetComponentLocation();
	TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

	RotationDelta = TargetRotation - LookAtRotation;
	RotationDelta.Normalize();
}

void ATurret::SetBeamLenght(float Lenght) {
	Beam->SetRelativeScale3D(FVector(Lenght / 200, Beam->GetRelativeScale3D().Y, Beam->GetRelativeScale3D().Z));
	Beam->SetRelativeLocation(FVector(Lenght / (-1.5), 0, 0));
}

void ATurret::BeamTrace() {
	FHitResult HitResult;
	FVector Start = TurretMeshComponent->GetSocketLocation("BeamSocket");
	FVector End = Start + Beam->GetForwardVector() * BeamLenght;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OUT HitResult, Start, End, ECC_Camera, CollisionQueryParams);
	if (bHit) {
		SetBeamLenght(HitResult.Distance);
	} else {
		SetBeamLenght(BeamLenght);
	}
}


void ATurret::RotateTowardsTarget(AActor* Target) {
	// Get the direction to the target
	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Normalize();

	// Calculate the new rotation
	FRotator NewRotation = Direction.Rotation();
	NewRotation.Pitch = 0.0f; // Keep the turret level
	NewRotation.Roll = 0.0f;

	// Rotate the turret
	FRotator CurrentRotation = FMath::RInterpConstantTo(GetActorRotation(), NewRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
	SetActorRotation(CurrentRotation);
}

void ATurret::Fire() {
	// Check if a projectile class has been set
	if (ProjectileClass != nullptr) {
		// Create a new projectile
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, FireLocation->GetComponentLocation(), FireLocation->GetComponentRotation());

		// Set the projectile's speed
		Projectile->SetSpeed(ProjectileSpeed);
	}
}
