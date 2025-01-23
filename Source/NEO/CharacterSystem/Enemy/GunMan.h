// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NEO/CharacterSystem/PlayerSystem/PlayerCharacter.h"
#include "Bullet.h"
#include "TrajectoryBullet.h"
#include "EnamyBase.h"
#include "GunMan.generated.h"

UCLASS()
class NEO_API AGunMan : public AEnamyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGunMan();
	UPROPERTY(EditAnywhere, Category = "AI")
		float DesiredDistance = 300.0f; // プレイヤーからの望ましい距離（3m）

	UPROPERTY(EditAnywhere, Category = "AI")
		float MovementSpeed = 100.0f; // 移動速度

	float MoveSpeed = 100.0f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FVector GetSnappedDirection(const FVector& Direction) const;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, Category = "Rotation")
		bool bIsRotation;
	FVector GetPlayerDirection() const;

	float GetDistanceToPlayer() const;
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class ATrajectoryBullet> TrajectoryBulletClass;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class ABullet> BulletClass;

	FTimerHandle BulletSpawnTimerHandle;
	FTimerHandle MovementResumeTimerHandle;
	FTimerHandle RotationLockTimerHandle;
	FTimerHandle AttackTimerHandle;
	FTimerHandle TickEnableTimerHandle;
	bool bIsSpawningBullet;
	bool bIsBulletAlive;
	bool bCanMove;
	FRotator LockedRotation;
	void SpawnTrajectoryBullet();
	void ReplaceWithBullet();
	void ResumeMovement();
	void UnlockRotation();
	void PlayAttackAnim();
	TOptional<FRotator> SavedRotation;
	void EnableTickAfterDelay();
	void CollisionOn();
};
