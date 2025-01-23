// Fill out your copyright notice in the Description page of Project Settings.


#include "NEOGameState.h"
#include "Kismet/GameplayStatics.h"
#include "NEO/CharacterSystem/PlayerSystem/NEOPlayerController.h"
#include "NEOGameMode.h"

// コンストラクタ
ANEOGameState::ANEOGameState()
	: IsReadyToUpdateGame(false)
{
}

/*
 * 関数名　　　　：InitGameState()
 * 処理内容　　　：ゲームの状態を初期化
 * 戻り値　　　　：なし
 */
void ANEOGameState::InitGameState()
{
	if (PlayerController) { return; }

	// タイトル状態に設定
	GameState = EGameState_NEO::OnTitle;

	// コントローラー取得
	PlayerController = Cast<ANEOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// プレイヤーの状態初期化
	PlayerController->ResetPlayerStatus();
}


/*
 * 関数名　　　　：InitInGame()
 * 処理内容　　　：インゲームの状態を更新
 * 戻り値　　　　：なし
 */
void ANEOGameState::InitInGame()
{
	// ゲームモード取得
	ANEOGameMode* pGameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (pGameMode)
	{
		// プレイヤーにカメラを設定
		pGameMode->InitCameraOnPlayer();
	}
}

/*
 * 関数名　　　　：ResetGame()
 * 処理内容　　　：インゲームの状態を更新
 * 戻り値　　　　：なし
 */
void ANEOGameState::RestartGame()
{
	// ゲームモード取得
	ANEOGameMode* pGameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (pGameMode)
	{
		// プレイヤーにカメラを設定
		pGameMode->RestartGame();
	}
}


/*
 * 関数名　　　　：UpdateGameState()
 * 処理内容　　　：ゲームの状態を更新
 * 引数１　　　　：float DeltaTime・・・毎フレームかかった時間
 * 戻り値　　　　：なし
 */
void ANEOGameState::UpdateGameState(float DeltaTime)
{
	switch (GameState)
	{
	case EGameState_NEO::OnTitle:
		OnTitle();
		break;
	case EGameState_NEO::OnOpening:
		OnOpening();
		break;
	case EGameState_NEO::OnGamePlaying:
		OnGamePlaying();
		break;
	case EGameState_NEO::OnGameClear:
		OnGameClear();
		break;
	case EGameState_NEO::OnGameOver:
		OnGameOver();
		break;
	default:

		break;

	}

}


/*
 * 関数名　　　　：OnTitle()
 * 処理内容　　　：タイトル画面にいるときの処理
 * 戻り値　　　　：なし
 */
void ANEOGameState::OnTitle()
{
	// ゲームの状態初期化
	InitGameState();

	if (!PlayerController) { return; }

	// タイトル画面が表示されているとき次に進める
	IsReadyToUpdateGame = (TitleState == ETitleState_NEO::OnTitleDisplay);

	// いずれかのキーが押されているか取得
	if (IsReadyToUpdateGame && PlayerController->GetIsAnyKeyPressed())
	{
		// 次のステートへ
		SetNextGameState(EGameState_NEO::OnOpening);

		// 表示されているWidgetは消去
		DeleteWidget();

		// タイトル状態に設定
		TitleState = ETitleState_NEO::OnDisplay_None;

		// 長押しなどで一気に飛ばないようにフラグを下げておく
		IsReadyToUpdateGame = false;
	}
}

/*
 * 関数名　　　　：OnOpening()
 * 処理内容　　　：オープニングの処理
 * 戻り値　　　　：なし
 */
void ANEOGameState::OnOpening()
{
	if (!PlayerController) { return; }

	// BP側でオープニング再生終了などを取得
	if (IsReadyToUpdateGame)
	{
		// 次のステートへ
		SetNextGameState(EGameState_NEO::OnGamePlaying);

		// フラグを下げておく
		IsReadyToUpdateGame = false;

		// インゲーム初期化
		InitInGame();
	}
}


/*
 * 関数名　　　　：OnGamePlaying()
 * 処理内容　　　：ゲームプレイ中の処理
 * 戻り値　　　　：なし
 */
void ANEOGameState::OnGamePlaying()
{
	if (!PlayerController) { return; }

	if (IsReadyToUpdateGame)
	{
		// プレイヤーが死んでいたらゲームオーバー
		if (PlayerController->GetPlayerIsDead())
		{
			// 次のステートへ
			SetNextGameState(EGameState_NEO::OnGameOver);
		}
		// それ以外はゲームクリア
		else
		{
			// 次のステートへ
			SetNextGameState(EGameState_NEO::OnGameClear);
		}

		// フラグを下げておく
		IsReadyToUpdateGame = false;
	}
}

/*
 * 関数名　　　　：OnGameClear()
 * 処理内容　　　：ゲームクリア画面の処理
 * 戻り値　　　　：なし
 */
void ANEOGameState::OnGameClear()
{
	if (IsReadyToUpdateGame)
	{
		// 次のステートへ
		SetNextGameState(EGameState_NEO::OnTitle);

		// ゲームリセット
		RestartGame();
	}	
}


/*
 * 関数名　　　　：OnGameOver()
 * 処理内容　　　：ゲームオーバー画面の処理
 * 戻り値　　　　：なし
 */
void ANEOGameState::OnGameOver()
{
	if (IsReadyToUpdateGame)
	{
		// 次のステートへ
		SetNextGameState(EGameState_NEO::OnTitle);

		// ゲームリセット
		RestartGame();
	}
}


/*
 * 関数名　　　　：SetNextGameState()
 * 処理内容　　　：ゲームを指定された状態へ
 * 戻り値　　　　：なし
 */
void ANEOGameState::SetNextGameState(EGameState_NEO _nextGameState)
{
	// 指定されたステートへ移動
	GameState = _nextGameState;

}