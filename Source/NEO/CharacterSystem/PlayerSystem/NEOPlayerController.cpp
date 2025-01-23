// Fill out your copyright notice in the Description page of Project Settings.


#include "NEOPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "NEO/GameSystem/NEOGameMode.h"
#include "PlayerBase.h"

ANEOPlayerController::ANEOPlayerController()
	: DefaultRemainingLives(2)
	, EnemiesCnt(3)
	, PlayerToEnemyDistance(200.f)
	, RespownPosHeight(300.f)
	, RemainingLives(DefaultRemainingLives)
	, PlayerIsDead(false)
{

}

// ゲーム開始時の処理
void ANEOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ゲームモード取得
	pGameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// プレイヤー取得
	pPlayer = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
}


/*
 * 関数名　　　　：ResetPlayerStatus()
 * 処理内容　　　：プレイヤーの入力受付(移動処理)
 * 引数１　　　　：FInputActionValue& Value・・・入力量
 * 戻り値　　　　：なし
 */
void ANEOPlayerController::ResetPlayerStatus()
{
	// 残機設定
	RemainingLives = DefaultRemainingLives;
}


/*
 * 関数名　　　　：DestroyPlayer()
 * 処理内容　　　：プレイヤーの入力受付(移動処理)
 * 戻り値　　　　：なし
 */
void ANEOPlayerController::DestroyPlayer()
{
	if (pPlayer && pGameMode)
	{
		// プレイヤー削除
		pPlayer->Destroy();

		// プレイヤーをNULLに
		pPlayer = nullptr;

		// プレイヤーを死亡状態へ
		PlayerIsDead = true;

		// ゲームを次の状態へ
		pGameMode->SetNextGameState();
	}
}


/*
 * 関数名　　　　：RespawnPlayer()
 * 処理内容　　　：プレイヤーの入力受付(移動処理)
 * 引数１　　　　：FInputActionValue& Value・・・入力量
 * 戻り値　　　　：なし
 */
void ANEOPlayerController::RespawnPlayer()
{
	if (pPlayer && pGameMode)
	{
		// プレイヤーが死亡した位置取得
		FTransform RespownTransForm = pPlayer->GetActorTransform();

		// プレイヤーを削除
		pPlayer->Destroy();

		// プレイヤーをNULLに
		pPlayer = nullptr;

		// プレイヤーの残機を1減らす
		ReduceRemainingLives();

		// 復活位置を少し高く
		const FVector RespownPos = RespownTransForm.GetTranslation();
		RespownTransForm.SetTranslation(FVector(RespownPos.X, RespownPos.Y, RespownPos.Z + RespownPosHeight));

		// 新しいプレイヤーを生成
		pPlayer = Cast<APlayerBase>(GetWorld()->SpawnActor<APawn>(pGameMode->GetDefaultPawnClass(), RespownTransForm));

		// リスポーンのカメラ処理
		pGameMode->SetCameraOnPlayer();
	}
}


/*
 * 関数名　　　　：GetPlayerLocation()
 * 処理内容　　　：プレイヤーの座標を返す
 * 戻り値　　　　：プレイヤーの座標
 */
FVector ANEOPlayerController::GetPlayerLocation()const
{
	if (pPlayer)
	{
		return pPlayer->GetActorLocation();
	}

	return FVector::ZeroVector;
}

/*
 * 関数名　　　　：GetNowCameraRotation()
 * 処理内容　　　：カメラの回転
 * 戻り値　　　　：なし
 */
FRotator ANEOPlayerController::GetNowCameraRotation()const
{
	if (pGameMode)
	{
		// 現在のカメラを取得
		AActor* NowCamera = pGameMode->GetNowPlayerCamera();

		// 回転を取得して返す
		if (NowCamera)
		{
			// カメラのコンポーネント取得
			UCameraComponent* CameraComponent = NowCamera->FindComponentByClass<UCameraComponent>();
			if (CameraComponent)
			{
				return CameraComponent->GetComponentRotation();
			}
		}
	}

	return FRotator::ZeroRotator;
}


/*
 * 関数名　　　　：GetIsDebugKeyPressed()
 * 処理内容　　　：デバッグキーに設定されているキーが押されているか
 * 戻り値　　　　：なし
 */
bool ANEOPlayerController::GetIsDebugKeyPressed()const
{
	for (int i = 0; i < sizeof(KeyNames) / sizeof(FName); ++i)
	{
		if (IsInputKeyDown(KeyNames[i]))
		{
			return false;
		}
	}

	return true;
}


/*
 * 関数名　　　　：AnyEnemiesNearbyMoreSecond()
 * 処理内容　　　：プレイヤーの近くに二体以上いるか判定
 * 戻り値　　　　：いたらtrue
 */
bool ANEOPlayerController::AnyEnemiesNearbyMoreSecond()const
{
	if (pGameMode)
	{
		// ゲームモードから現在出ている敵の情報を取得
		TArray<AActor*> Enemies = pGameMode->GetEnemies();

		// プレイヤーの位置取得
		FVector PlayerPos = pPlayer->GetActorLocation();

		// 近くに敵がいた時のカウント
		int NearByEnemiesCnt = 0;
		// プレイヤーと敵との距離を計測
		for (int i = 0; i < Enemies.Num(); ++i)
		{
			if (Enemies[i])
			{
				// 敵の位置取得
				FVector EnemyPos = Enemies[i]->GetActorLocation();
				// プレイヤーと距離計算
				float Distance = FVector::Dist(EnemyPos, PlayerPos);

				if (Distance <= PlayerToEnemyDistance)
				{
					++NearByEnemiesCnt;
				}
			}

			if (NearByEnemiesCnt >= EnemiesCnt)
			{
				return true;
			}
		}
	}

	return false;
}