// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NEOPlayerController.generated.h"

class APlayerBase;

UCLASS()
class NEO_API ANEOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// コンストラクタ
	ANEOPlayerController();

	// ゲーム開始時の処理
	void BeginPlay();

	// いずれかのキーを入力中かを取得
	UFUNCTION(BlueprintCallable, Category = "SetStatus")
		bool GetIsAnyKeyPressed()const { return IsInputKeyDown("AnyKey") && GetIsDebugKeyPressed(); }

	UFUNCTION(BlueprintCallable, Category = "SetStatus")
		bool AnyEnemiesNearbyMoreSecond()const;

	// プレイヤーの状態リセット
	void ResetPlayerStatus();

	// プレイヤーの残機取得
	int GetRemainingLives()const { return RemainingLives; }

	// プレイヤーの残機減らす
	void ReduceRemainingLives() { --RemainingLives; }

	// プレイヤーの削除
	void DestroyPlayer();

	// プレイヤーのリスポーン
	void RespawnPlayer();

	// プレイヤーの位置取得
	FVector GetPlayerLocation()const;

	// 現在のプレイヤーのカメラ取得
	FRotator GetNowCameraRotation()const;

	// プレイヤーが死んでいるかを伝える
	bool GetPlayerIsDead()const { return PlayerIsDead; }

private:

	// デバッグキーが押されたか
	bool GetIsDebugKeyPressed()const;

private:

	// プレイヤーがリスポーンできる回数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int DefaultRemainingLives;

	// プレイヤーの近くに何体敵がいたら強調表示するか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		int EnemiesCnt;

	// プレイヤーがリスポーンできる回数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float PlayerToEnemyDistance;

	// リスポーン時の高さ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float RespownPosHeight;

	// PressAnyからFキーは除く
	FName KeyNames[10] = { "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10" };

	// プレイヤーの復活可能回数
	int RemainingLives;

	// プレイヤーが死んでいるか
	bool PlayerIsDead;

	// プレイヤーの情報
	APlayerBase* pPlayer;

	// ゲームモード
	class ANEOGameMode* pGameMode;
};

