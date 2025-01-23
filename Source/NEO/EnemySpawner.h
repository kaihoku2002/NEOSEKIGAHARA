// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NEO/CharacterSystem/Enemy/EnamyBase.h" // スポーンするEnemyキャラクターのヘッダーファイルをインクルードしてください
#include "EnemySpawner.generated.h"

UCLASS()
class NEO_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Spawning")
        TSubclassOf<AEnamyBase> EnemyClass;

    UPROPERTY(EditAnywhere, Category = "Spawning")
        int32 NumEnemiesToSpawn;

   

    void SpawnEnemies();
};
