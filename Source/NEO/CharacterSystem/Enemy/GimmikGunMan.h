// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunMan.h"
#include "GimmikGunMan.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API AGimmikGunMan : public AGunMan
{
	GENERATED_BODY()
public:
	AGimmikGunMan();
	virtual void Tick(float DeltaTime) override;
};
