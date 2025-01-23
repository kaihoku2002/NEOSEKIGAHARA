#include "EnemySpawner.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    SpawnEnemies();
    NumEnemiesToSpawn = 5;
}

void AEnemySpawner::SpawnEnemies()
{
        UWorld* World = GetWorld();
        if (World && EnemyClass)
        {
            for (int32 i = 0; i < NumEnemiesToSpawn; i++)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                AEnamyBase* SpawnedEnemy = GetWorld()->SpawnActor<AEnamyBase>(EnemyClass, GetActorLocation(), GetActorRotation(), SpawnParams);

                if (SpawnedEnemy)
                {
                    // ¬Œ÷‚µ‚½ê‡‚Ìˆ—‚ğ‚±‚±‚É‹Lq
                }
            }
         }
}