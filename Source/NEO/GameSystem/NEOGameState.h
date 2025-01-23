// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NEOGameState.generated.h"


// ゲームの状態を管理するEnum
UENUM(BlueprintType)
enum class EGameState_NEO : uint8
{
	OnTitle			UMETA(DisplayName = "Title"),			// タイトル画面
	OnOpening		UMETA(DisplayName = "Opening"),			// オープニング
	OnGamePlaying	UMETA(DisplayName = "InGame"),			// インゲーム
	OnGameClear		UMETA(DisplayName = "GameClear"),		// クリア
	OnGameOver		UMETA(DisplayName = "GameOver")			// オーバー
};

// タイトルの状態を管理するEnum
UENUM(BlueprintType)
enum class ETitleState_NEO : uint8
{
	OnDisplay_None	UMETA(DisplayName = "None"),			// なにもしない
	OnLogoDisplay	UMETA(DisplayName = "LogoMovie"),		// 日電のロゴ表示
	OnTitleDisplay	UMETA(DisplayName = "Title"),			// タイトル画面
	OnDemoDisplay	UMETA(DisplayName = "Demo"),			// デモ画面
};


UCLASS()
class NEO_API ANEOGameState : public AGameStateBase
{
	GENERATED_BODY()


	// コンストラクタ
	ANEOGameState();

public:

	// ゲームの状態を更新
	void UpdateGameState(float DeltaTime);

	// ゲームステート更新処理
	UFUNCTION(BlueprintCallable, Category = "GetParam")
		class ANEOPlayerController* GetPlayerController()const { return PlayerController; }

	// ゲームステート取得
	UFUNCTION(BlueprintCallable, Category = "UpdateState")
		EGameState_NEO GetGameState()const { return GameState; }

	// タイトルステート取得
	UFUNCTION(BlueprintCallable, Category = "UpdateState")
		ETitleState_NEO GetTitleState()const { return TitleState; }

	// タイトルステートセット
	UFUNCTION(BlueprintCallable, Category = "UpdateState")
		void SetTitleState(ETitleState_NEO _titleState){ TitleState = _titleState; }

	// 次のステートへ移動する準備が整ったときに使用
	UFUNCTION(BlueprintCallable, Category = "UpdateState")
		void SetReadyUpdateGame(bool _isReadyToUpdateGame){ IsReadyToUpdateGame = _isReadyToUpdateGame; }

	// ゲームを任意の状態へ
	UFUNCTION(BlueprintCallable, Category = "UpdateState")
		void SetNextGameState(EGameState_NEO _nextGameState);

	// タイトル画面を表示
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Title")
		void DisplayTitleScreen();

	virtual void DisplayTitleScreen_Implementation() {};

	// デモ画面を表示
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "DEMO")
		void DisplayDemoScreen();

	virtual void DisplayDemoScreen_Implementation() {};

	// デモ画面を表示
	UFUNCTION(BlueprintImplementableEvent)
		void DeleteWidget();

private:

	// ゲームの状態を初期化
	void InitGameState();	

	// ゲームの状態をリセット
	void RestartGame();

	// インゲーム開始時にする処理
	void InitInGame();

	// タイトルの処理
	void OnTitle();

	// オープニングの処理
	void OnOpening();

	// インゲームの処理
	void OnGamePlaying();

	// クリアの処理
	void OnGameClear();

	// オーバーの処理
	void OnGameOver();	

private:

	// ゲームの状態を管理
	EGameState_NEO GameState;

	// タイトルの状態を管理
	ETitleState_NEO TitleState = ETitleState_NEO::OnLogoDisplay;

	// ゲームを次の状態にアップデートする準備ができたか
	bool IsReadyToUpdateGame;

	// プレイヤーのコントローラー格納用
	class ANEOPlayerController* PlayerController;
};
