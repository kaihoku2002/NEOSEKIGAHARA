// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NEOGameState.h"
#include "NEOGameMode.generated.h"

UCLASS()
class NEO_API ANEOGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ANEOGameMode();

	// ゲーム開始時の処理
	void BeginPlay() override;

	// 毎フレーム更新
	void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
		bool GetIsBattleArea()const { return bIsOnBattleArea; }

	UFUNCTION(BlueprintCallable, Category = "UpdateState")
		class ANEOGameState* GetGameState()const { return pGameState; }

	// プレイヤーのカメラ変更
	UFUNCTION(BlueprintCallable, Category = "UpdateState")
		void SetViewTargetWithBlend(AActor* _newViewTarget, float _blendTime = 0.f, EViewTargetBlendFunction _blendFunc = VTBlend_Linear, float _blendExp = 0.f, bool _bLockOutgoing = false);

	// プレイヤーのカメラ設定
	void SetPlayerCamera(AActor* _playerCamera) { PlayerCamera = _playerCamera; }

	void InitCameraOnPlayer();
	
	// 現在のカメラ取得
	AActor* GetNowPlayerCamera()const;

	// イベント進行度取得
	int32 GetEventIndex() { return EventIndex; }

	UFUNCTION(BlueprintCallable, Category = "Area")
		void SetIsOnBattleArea(bool _IsBattleArea,TArray<class ASpawnPoint*> SpawnPoints,
			AActor* Camera,
			class UProceduralMeshComponent* LeftMesh,
			class UProceduralMeshComponent* RightMesh,
			class UProceduralMeshComponent* NearMesh,
			class UProceduralMeshComponent* FarMesh, 
			float SetWallDealy = 0.f
		);

	void SetWall();

	TArray<AActor*> GetEnemies()const { return Enemies; }


	// バトルエリアから出る
	void ExitBattleArea();

	//バトルエリアのカメラ
	AActor* BattleAreaCamera;

	TArray<class UProceduralMeshComponent*> BattleAreaMeshs;

	TArray<class ASpawnPoint*> BattleAreaSpawnPoints;


	// 敵の出現処理
	AActor* SpawnEnemy(ASpawnPoint* spawnPoint);

	// バトルエリア内に敵を出現させる
	void SpawnEnemyInBattleArea();

	// ゲームを次の状態へ移す準備が整ったことを伝える
	void SetNextGameState() { if (pGameState) { pGameState->SetReadyUpdateGame(true); } }

	// プレイヤーにカメラを設定する
	void SetCameraOnPlayer();

	// エネミーの削除
	void DestroyEnemy(AActor* _enemy, bool _bBattleAreaEnemy);

	// デフォルトポーンクラスを取得
	TSubclassOf<APawn> GetDefaultPawnClass()const { return DefaultPawnClass; }

	// ゲームリセット
	void RestartGame();

	// ゲームポーズ
	void SetGamePause(bool _bPaused);

	// デモ画面を表示
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void DestroyerEvent();


private:

	TArray<AActor*> Enemies;

	// バトルエリアのフラグ
	bool bIsOnBattleArea;

	// イベント用のインデックス
	int32 EventIndex;

	// バトルエリア内の敵のカウンター
	int32 BattleAreaEnemyCount = 0;

	// プレイヤーのカメラ格納用(移動するカメラ)
	AActor* PlayerCamera;

	// プレイヤーのカメラ(カメラ固定時)
	AActor* pCamera;

	// ゲームステートのポインタ
	class ANEOGameState* pGameState;

	// プレイヤーコントローラーのポインタ
	class ANEOPlayerController* PlayerController;

	class ULevelSequence* BossStartMovie;

	FTimerHandle TimerHandle;
};
