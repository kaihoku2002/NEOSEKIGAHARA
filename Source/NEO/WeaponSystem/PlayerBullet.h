// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerBullet.generated.h"

UCLASS()
class NEO_API APlayerBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 接触開始時に行う処理
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 攻撃力セット
	void SetDamageAmount(int32 _damageAmount) { DamageAmount = _damageAmount; }

private:

	// 弾の移動処理
	void Move();

	// 削除までの時間に達していたら
	void SetDestroyFlag();

	// 弾のDestroy
	void DestroyBullet(bool _destroyFlag = false);

private:

	// 弾のコリジョン
	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* BulletCollision;

	// 武器のメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BulletMesh", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* BulletStaticMesh;

	// 弾の移動速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BulletParams", meta = (AllowPrivateAccess = "true"))
		float MoveSpeed;

	// 攻撃力
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BulletParams", meta = (AllowPrivateAccess = "true"))
		float DamageAmount;

	// 発射から削除までの時間
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BulletParams", meta = (AllowPrivateAccess = "true"))
		float TimeToDelete;

	// タイマーハンドル
	FTimerHandle DeleteTimerHandle;
};
