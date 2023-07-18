// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterInterface.h"
#include "GameFramework/Pawn.h"
#include "NiagaraSystem.h"
#include "MyPawn.generated.h"

UCLASS()
class VR_TEST_WORK_API AMyPawn : public APawn, public ICharacterInterface {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsEnemy_Implementation() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere)
	float Health = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* HitEffect;
	
};
