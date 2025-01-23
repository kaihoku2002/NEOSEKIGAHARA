// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionAssistComponent.generated.h"


UCLASS(ClassGroup = (ActionAssist), meta = (BlueprintSpawnableComponent))
class NEO_API UActionAssistComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// コンストラクタ
	UActionAssistComponent();

	// 攻撃の角度修正
	void CorrectAttackAngle();

	// ヒットストップ処理
	void HitStop(float _speedDuringHitStop, float _stopTime);

	// エフェクトスポーン
	void SpawnEffect(class UNiagaraSystem* _hitParticle, FVector _spawnPos);

	// オーナーの向きを設定
	void OwnerParallelToCamera(bool _lookRight);

	// カメラシェイク
	void CameraShake(TSubclassOf<UCameraShakeBase> _shakePattern, float _scale = 1.f);

	// サウンド再生
	void PlaySound(class USoundBase* _sound_Obj, float StartTime = 0);

	// レイを飛ばす
	bool WallCheck(float _lineLength);

protected:

	// ゲーム開始時に呼び出される処理
	virtual void BeginPlay() override;

	// 毎フレーム呼び出される処理
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	// 前方にいるActorを返す
	AActor* GetFrontActor();

	// ヒットストップ終了
	void EndHitStop();

	// オーナーの次の傾き取得
	double GetAngle_Yaw(bool _lookRight, double _camera_Yaw);
	double GetAngle_Pitch(bool _lookRight, double _camera_Pitch);

protected:

	//-------------各機能のON・OFF---------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Assist")
		bool bUseCorrectAttackAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Assist")
		bool bUseHitStop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Assist")
		bool bUseHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Assist")
		bool bUseFaceCamera;

	//----------------------------------------------------------------------------

	// 攻撃の角度補正用のレイの長さ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Assist")
		float RayLength_CorrectAngle;

	// 攻撃の角度補正の最大角度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Assist")
		float MaxCorrectAngle;

private:

	// タイマーハンドル
	FTimerHandle TimerHandle_HitStop;

	// アニメーションプレイ中かどうか
	bool IsAnimationPlaying;
};
