// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "TrajectoryBullet.generated.h"

UCLASS()
class NEO_API ATrajectoryBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrajectoryBullet();
    UPROPERTY(VisibleAnywhere)
        UNiagaraComponent* NiagaraComponent;

    UPROPERTY(EditAnywhere, Category = "Effects")
        UNiagaraSystem* NiagaraSystemAsset;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    FTimerHandle& GetLifeSpanTimerHandle()
    {
        return LifeSpanTimerHandle;
    };
    float GetLifeSpan() const
    {
        return LifeSpan;
    }
private:
    
   
    FTimerHandle LifeSpanTimerHandle;
    float LifeSpan = 2.0f;
    
};
