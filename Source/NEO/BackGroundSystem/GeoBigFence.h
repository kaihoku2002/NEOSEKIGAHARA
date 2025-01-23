// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "NiagaraComponent.h"
#include "GeoBigFence.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API AGeoBigFence : public AGeometryCollectionActor
{
	GENERATED_BODY()
public:
	AGeoBigFence();
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Effects")
		UNiagaraComponent* NiagaraComponent;

};
