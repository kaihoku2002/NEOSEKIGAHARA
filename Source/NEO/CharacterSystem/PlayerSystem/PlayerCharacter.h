// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class NEO_API APlayerCharacter : public APlayerBase
{
	GENERATED_BODY()


private:

	// コンストラクタ
	APlayerCharacter();

	// ゲーム開始時に呼び出される処理
	virtual void BeginPlay() override;

	// 毎フレーム呼び出される処理
	virtual void Tick(float DeltaTime) override;
};
