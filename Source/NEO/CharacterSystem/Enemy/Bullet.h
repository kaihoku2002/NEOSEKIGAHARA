// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrajectoryBullet.h"
#include "Components/SphereComponent.h"

#include "Bullet.generated.h"

UCLASS()
class NEO_API ABullet : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABullet();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    void EnableCollision();

    UFUNCTION()
        void OnOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    ATrajectoryBullet* GetTrajectoryBullet() const
    {
        return TrajectoryBullet;
    }

    void SetTrajectoryBullet(ATrajectoryBullet* Bullet)
    {
        TrajectoryBullet = Bullet;
    }

private:
 

    // Bullet lifespan
    UPROPERTY(EditAnywhere)
        float Lifespan;
    
    UPROPERTY(EditAnywhere)
        float DamageAmount;

    // Movement direction
    FVector Direction;
    // Sphere component
    UPROPERTY(VisibleAnywhere)
        USphereComponent* SphereComponent;

    // Collision component
    UPROPERTY(VisibleAnywhere)
        UPrimitiveComponent* CollisionComponent;
    ATrajectoryBullet* TrajectoryBullet;
   
};

