// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "Components/StaticMeshComponent.h"
#include "ObjectBase.generated.h"

UCLASS()
class NEO_API AObjectBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
    AObjectBase();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
        UStaticMeshComponent* Mesh;

    UPROPERTY()
        class ACharacter* PlayerCharacter; // プレイヤーキャラクターの参照

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
        float  Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
        float  MaxHealth;

    UPROPERTY(EditDefaultsOnly, Category = "Destruction")
        TSubclassOf<AGeometryCollectionActor> GeometryCollectionClass;

    void ReceiveDamage(float DamageAmount);

    void CheckAndDestroy();
   
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	AActor* GetPlayer();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
