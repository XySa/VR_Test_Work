// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "CharacterInterface.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TurretAnimInterface.h"
#include "Particles/ParticleSystemComponent.h"

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

	PMuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleFlash"));
	PMuzzleFlash->SetupAttachment(TurretMeshComponent, TEXT("BeamSocket"));
	PMuzzleFlash->SetAutoActivate(false);
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

	if (Enemy) {
		FollowEnemy(DeltaTime);
	} else {
		UpdateLookTarget(DeltaTime);
	}
}

void ATurret::FollowEnemy(float DeltaTime) {
	FVector Start = TurretMeshComponent->GetSocketLocation("BeamSocket");
	FVector End = Enemy->GetActorLocation();

	FRotator RotationToEnemy = UKismetMathLibrary::FindLookAtRotation(Start, End);

	LookAtRotation = FMath::RInterpTo(LookAtRotation, RotationToEnemy, DeltaTime, 10);

	if (TurretMeshComponent->GetAnimInstance()->Implements<UTurretAnimInterface>()) {
		ITurretAnimInterface::Execute_UpdateLookAtRotation(TurretMeshComponent->GetAnimInstance(), LookAtRotation);
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
	Beam->SetRelativeLocation(FVector(Lenght / -1.5, 0, 0));
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
		CheckEnemy(HitResult.GetActor());
	} else {
		SetBeamLenght(BeamLenght);
	}
}

void ATurret::CheckEnemy(AActor* HitActor) {
	if (HitActor->Implements<UCharacterInterface>()) {
		bool bEnemy = ICharacterInterface::Execute_IsEnemy(HitActor);
		if (bEnemy) {
			if (!Enemy) {
				Enemy = HitActor;
			}
			//Start Shooting
			GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &ATurret::Fire, .4f, true, .4f);
			UE_LOG(LogTemp, Warning, TEXT("Enemy"));
		}
	} else {
		if (Enemy) {
			//Stop shooting
			//Enemy = nullptr;
			//GetWorldTimerManager().ClearTimer(ShootTimerHandle);
		}
	}
}


void ATurret::Fire() {
	//Shoot Turret Projectile
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, PMuzzleFlash->GetComponentLocation());
	PMuzzleFlash->Activate(true);

	FHitResult HitResult;
	FVector Start = TurretMeshComponent->GetSocketLocation("BeamSocket");
	FVector End = Start + Beam->GetForwardVector() * BeamLenght;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OUT HitResult, Start, End, ECC_Camera, CollisionQueryParams);
	if (bHit) {
		FPointDamageEvent PointDamageEvent(10.f, HitResult, Beam->GetForwardVector(), nullptr);
		HitResult.GetActor()->TakeDamage(10.f, PointDamageEvent, GetInstigatorController(), this);
	}
}
