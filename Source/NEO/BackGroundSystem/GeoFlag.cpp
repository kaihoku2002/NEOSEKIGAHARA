// Fill out your copyright notice in the Description page of Project Settings.


#include "GeoFlag.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "UObject/ConstructorHelpers.h"
AGeoFlag::AGeoFlag()
{
    // ジオメトリコレクションアセットを設定
    static const ConstructorHelpers::FObjectFinder<UGeometryCollection> GeoCollection(TEXT("GeometryCollection'/Game/0102/Enemy/Poal_GeometryCollection_GeometryCollection.Poal_GeometryCollection_GeometryCollection'"));
    if (GeoCollection.Succeeded())
    {
        GetGeometryCollectionComponent()->RestCollection = GeoCollection.Object;
    }

    

}


void AGeoFlag::BeginPlay()
{
    Super::BeginPlay();
    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw = 0.0f;
    SetActorRotation(NewRotation);
}
