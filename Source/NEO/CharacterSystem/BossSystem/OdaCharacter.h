// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OdaBase.h"
#include "OdaCharacter.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API AOdaCharacter : public AOdaBase
{
	GENERATED_BODY()

		//必殺技時の衝撃波のダメージ
		float UltShockWaveDamage;

	//必殺技のダメージ
		float UltDamage;

	void OdaUlt(int Timer);
};
