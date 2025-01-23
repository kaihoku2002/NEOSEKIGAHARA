// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pusher.h"
#include "Engine/TargetPoint.h"
#include "Components/BoxComponent.h"
#include "RockFlag.generated.h"

UCLASS()
class NEO_API ARockFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARockFlag();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		UBoxComponent* FlagBox;
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class APusher> PusherClass;
	UPROPERTY(EditAnywhere, Category = "Spawning")
		ATargetPoint* SpawnPointOne;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		ATargetPoint* SpawnPointTwo;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool bIsOnStep;
	void SpawnPusher();
};
