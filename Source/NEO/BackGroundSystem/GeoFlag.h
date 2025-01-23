// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeoFlag.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API AGeoFlag : public AGeometryCollectionActor
{
	GENERATED_BODY()
public:
	AGeoFlag();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
