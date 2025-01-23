// Fill out your copyright notice in the Description page of Project Settings.


#include "FenceOfGunMan.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NEO/CharacterSystem/PlayerSystem\PlayerCharacter.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Bullet.h"
#include "TrajectoryBullet.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NEO/BackGroundSystem/GunFence.h"
#include "NavigationPath.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AFenceOfGunMan::AFenceOfGunMan()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // キャラクターの移動方式を設定
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
    MaxHealth = 100;
    Health = MaxHealth;

    BulletSpawnTimerHandle = FTimerHandle();
}


// Called when the game starts or when spawned
void AFenceOfGunMan::BeginPlay()
{
    Super::BeginPlay();
    // プレイヤーキャラクターの参照を取得
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Fence"), FoundActors);

    if (FoundActors.Num() > 0)
    {
        AActor* NearestActor = FoundActors[0];
        float MinDistance = FVector::Dist(NearestActor->GetActorLocation(), GetActorLocation());

        // Loop through the rest of the found Actors
        for (int32 i = 1; i < FoundActors.Num(); i++)
        {
            float Distance = FVector::Dist(FoundActors[i]->GetActorLocation(), GetActorLocation());

            // If this Actor is closer than the current closest, update our 'nearest' Actor
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                NearestActor = FoundActors[i];
            }
        }

        // Set the nearest Actor as the target
        TargetActor = NearestActor;
    } 
    GetWorldTimerManager().SetTimer(BulletSpawnTimerHandle, this, &AFenceOfGunMan::SpawnTrajectoryBullet, 3.0f, true);
}
   




// Called every frame
void AFenceOfGunMan::Tick(float DeltaTime)
{

    Super::Tick(DeltaTime);
    if (TargetActor)
    {
        FVector Direction = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Direction);
    }
}

void AFenceOfGunMan::SpawnTrajectoryBullet()
{
    // Spawn a TrajectoryBullet
    FActorSpawnParameters SpawnParams;
    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();

    ATrajectoryBullet* Bullet = GetWorld()->SpawnActor<ATrajectoryBullet>(TrajectoryBulletClass, SpawnLocation, SpawnRotation, SpawnParams);
    if (Health > 0)
    {
        if (Bullet)
        {
            // Set up timer to replace TrajectoryBullet with Bullet after 2 seconds
            GetWorldTimerManager().SetTimer(Bullet->GetLifeSpanTimerHandle(), this, &AFenceOfGunMan::ReplaceWithBullet, 2.0f, false);
        }

    }


    // Prevent movement for the lifespan of the TrajectoryBullet + 1 second
    GetCharacterMovement()->Deactivate();
    
}

void AFenceOfGunMan::ReplaceWithBullet()
{
    // Replace TrajectoryBullet with Bullet
    FActorSpawnParameters SpawnParams;
    FVector SpawnLocation = GetActorLocation(); // Adjust as necessary
    FRotator SpawnRotation = GetActorRotation(); // Adjust as necessary

    ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnLocation, SpawnRotation, SpawnParams);
    if (Bullet)
    {
        // Destroy the TrajectoryBullet if it's valid
        ATrajectoryBullet* TrajectoryBullet = Bullet->GetTrajectoryBullet();
        if (IsValid(TrajectoryBullet))
        {
            GetWorld()->DestroyActor(TrajectoryBullet);
        }
    }
}
void AFenceOfGunMan::ResumeMovement()
{
    // Resume movement
    GetCharacterMovement()->Activate(true);
}
