// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Turret.generated.h"

UCLASS(BlueprintType, Blueprintable)

class VR_TEST_WORK_API ATurret : public AActor {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USceneComponent* TurretRootComponent;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* TurretMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Beam;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Target1;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Target2;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* FollowTarget;

	UPROPERTY(EditAnywhere)
	float ChangeTargetDelay = 5.f;

	UPROPERTY(EditAnywhere)
	float RotationRateMultiplayer = 1.f;


	FTimerHandle ChangeTargetTimerHandle;
	FTimerHandle TraceTimerHandle;

	/* Rotation related variables**/
	int TimerCount = 0;
	FRotator LookAtRotation;
	FRotator TargetRotation;
	FRotator RotationDelta;

	
	void UpdateLookTarget(float DeltaTime);
	void ChangeBeamTarget();

	UFUNCTION(BlueprintCallable)
	void SetBeamLenght(float Lenght);

	UFUNCTION()
	void BeamTrace();

public:
	// Sets default values for this actor's properties
	ATurret();

	/* Beam lenght**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BeamLenght = 1000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function to rotate the turret towards the target
	void RotateTowardsTarget(AActor* Target);

	// Function to fire a projectile
	void Fire();

	// The speed at which the turret rotates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float RotationSpeed;

	// The projectile to fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	TSubclassOf<class AProjectile> ProjectileClass;

	// The speed of the projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float ProjectileSpeed;

	// The location from which the projectile is fired
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	USceneComponent* FireLocation;

	// The range within which the turret can detect targets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float DetectionRange;

	// The time between shots
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turret")
	float FireRate;

private:
	// The target that the turret is aiming at
	AActor* OutTarget;

	// The time since the turret last fired
	float TimeSinceLastShot;
};
