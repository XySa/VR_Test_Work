// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/SphereComponent.h"


// Sets default values
AProjectile::AProjectile() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set default speed
	Speed = 1000.0f;

	// Set default damage
	Damage = 10.0f;

	// Create a collision component
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(15.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Projectile"));

	// Set up a notification for when this component hits something
	SphereComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::SetSpeed(float NewSpeed) {
	Speed = NewSpeed;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay() {
	Super::BeginPlay();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	// Only apply the damage if we hit a character
	if (OtherActor != nullptr && OtherActor->IsA(APawn::StaticClass())) {
		OtherActor->TakeDamage(Damage, FDamageEvent(UDamageType::StaticClass()), nullptr, this);
	}

	// Destroy the projectile
	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Move the projectile
	FVector NewLocation = GetActorLocation() + (DeltaTime * Speed * GetActorForwardVector());
	SetActorLocation(NewLocation);
}
