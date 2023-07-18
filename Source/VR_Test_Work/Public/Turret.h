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

	UPROPERTY(EditDefaultsOnly, Category = "Shooting")
	UParticleSystemComponent* PMuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = "Shooting")
	USoundBase* FireSound;
	
	UPROPERTY()
	AActor* Enemy = nullptr;

	FTimerHandle ChangeTargetTimerHandle;
	FTimerHandle TraceTimerHandle;
	FTimerHandle ShootTimerHandle;

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

	UFUNCTION()
	void CheckEnemy(AActor* HitActor);

	UFUNCTION()
	void FollowEnemy(float DeltaTime);

	UFUNCTION()
	void Fire();

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
};
