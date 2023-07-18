// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "NiagaraFunctionLibrary.h"


// Sets default values
AMyPawn::AMyPawn() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AMyPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool AMyPawn::IsEnemy_Implementation() {
	if (Health > 0) {
		return true;
	}
	return false;
}


float AMyPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	if (Health <= 0) {
		return 0;
	}
	float DamageCaused = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageCaused = FMath::Min(Health, DamageCaused);
	Health -= DamageCaused;

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		FVector HitLocation = ((FPointDamageEvent*)&DamageEvent)->HitInfo.Location;
		FRotator HitRotation = ((FPointDamageEvent*)&DamageEvent)->HitInfo.Normal.Rotation();
		UNiagaraComponent* BloodEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitEffect, HitLocation, HitRotation, FVector(1, 1, 1), true, true,
																						ENCPoolMethod::AutoRelease, true);
	}


	if (Health <= 0.0f) {
		// We're dead, don't allow further damage
		UE_LOG(LogTemp, Warning, TEXT("Pawn died"));
		DisableInput(GetWorld()->GetFirstPlayerController());
		auto World = GetWorld();
		auto PlayerController = World->GetFirstPlayerController();
		World->ServerTravel(World->GetAddressURL());
	}

	return DamageCaused;
}
