// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NEO/CharacterSystem/PlayerSystem/PlayerCharacter.h"
#include "Bullet.h"
#include "TrajectoryBullet.h"
#include "EnamyBase.h"
#include "FenceOfGunMan.generated.h"

UCLASS()
class NEO_API AFenceOfGunMan : public AEnamyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFenceOfGunMan();

	UPROPERTY(EditAnywhere, Category = "AI")
		float MovementSpeed = 100.0f; // ˆÚ“®‘¬“x


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Rotation")
		bool bIsRotation;
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class ATrajectoryBullet> TrajectoryBulletClass;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class ABullet> BulletClass;

	FTimerHandle BulletSpawnTimerHandle;
	FTimerHandle MovementResumeTimerHandle;

	void SpawnTrajectoryBullet();
	void ReplaceWithBullet();
	void ResumeMovement();
	AActor* TargetActor;
};
