// Fill out your copyright notice in the Description page of Project Settings.


#include "TrajectoryBullet.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
// Sets default values
ATrajectoryBullet::ATrajectoryBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));

    // Attach Niagara Component to Root
    RootComponent = NiagaraComponent;
    

}

// Called when the game starts or when spawned
void ATrajectoryBullet::BeginPlay()
{
	Super::BeginPlay();
    
    if (NiagaraSystemAsset)
    {
        // Set the asset for the Niagara Component
        NiagaraComponent->SetAsset(NiagaraSystemAsset);

        // Activate the Niagara System
        NiagaraComponent->Activate();
    }

    GetWorldTimerManager().SetTimer(LifeSpanTimerHandle, [this]()
        {
            Destroy();
        }, LifeSpan, false);
   
}

// Called every frame
void ATrajectoryBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




