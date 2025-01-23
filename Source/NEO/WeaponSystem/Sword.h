// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Sword.generated.h"

/**
 * 
 */
UCLASS()
class NEO_API ASword : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASword();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	// 当たり判定
	void SetCollision();

	// プレイヤーの当たり判定
	void PlyerAttack();

	// 敵の当たり判定
	void EnemyAttack();

	// ボスの攻撃記述用
	void BossAttack();

	// 敵に攻撃した時のヒットストップの時間を設定
	float SetHitStopTime(int _comboNum);

private:

	// 被ダメージ時のエフェクト
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
		class UNiagaraSystem* HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
		class USoundBase* EnemyHitSoundObj;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
		class USoundBase* ObjectHitSoundObj;

	// 何秒間ヒットストップを起こすか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Action Assist")
		float HitStopTime = 0.2f;

	// ヒットストップの長さの上がり幅
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Action Assist")
		float HitStopTimeRise = 0.05f;
};
