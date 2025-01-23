// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectBase.h"
#include "Engine/TargetPoint.h"
#include "GunFence.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API AGunFence : public AObjectBase
{
	GENERATED_BODY()
public:
	AGunFence();
	UPROPERTY(EditAnywhere)
		TArray<ATargetPoint*> TargetPoints;
	ATargetPoint* FindNearestAvailableTargetPoint(const AActor* Actor);
};
