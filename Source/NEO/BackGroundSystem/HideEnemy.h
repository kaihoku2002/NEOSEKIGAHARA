// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectBase.h"
#include "NiagaraComponent.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "HideEnemy.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API AHideEnemy : public AActor
{
	GENERATED_BODY()
public:
	AHideEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		UBoxComponent* HidingBox;
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


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
