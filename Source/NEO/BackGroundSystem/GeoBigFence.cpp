// Fill out your copyright notice in the Description page of Project Settings.


#include "GeoBigFence.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "UObject/ConstructorHelpers.h"
AGeoBigFence::AGeoBigFence()
{
    static const ConstructorHelpers::FObjectFinder<UGeometryCollection> GeoCollection(TEXT("GeometryCollection'/Game/0102/Enemy/Big_Fence_GeometryCollection.Big_Fence_GeometryCollection'"));
    if (GeoCollection.Succeeded())
    {
        GetGeometryCollectionComponent()->RestCollection = GeoCollection.Object;
    }

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(RootComponent);
}

void AGeoBigFence::BeginPlay()
{
    Super::BeginPlay();
    // Niagara particle system‚ðÄ¶
    if (NiagaraComponent)
    {
        NiagaraComponent->Activate();
        NiagaraComponent->SetPaused(false);
    }
}
