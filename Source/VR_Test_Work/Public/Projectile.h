// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AProjectile : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();

	// Function to set the speed of the projectile
	void SetSpeed(float NewSpeed);

	// The amount of damage the projectile deals
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Damage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The speed of the projectile
	float Speed;

	// Function to handle collision with other actors
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
