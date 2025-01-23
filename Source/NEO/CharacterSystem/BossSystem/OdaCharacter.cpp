// Fill out your copyright notice in the Description page of Project Settings.


#include "OdaCharacter.h"

/*
 * 関数名　　　　：OdaUlt()
 * 処理内容　　　：必殺技攻撃をするための関数
 * 戻り値　　　　：なし
 */
void AOdaCharacter::OdaUlt(int Timer)
{

	if (Timer % 30 == 0 && isUltShotTiming != true)
	{
		isUltShotTiming = true;
	}
	//軸を合わせに行く
	if (isUltShotTiming != true)
	{
		if (GetActorLocation().Y > UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
		{
			BossMoveY(OdaSpeed.Flont);
		}
		else
		{
			BossMoveY(-OdaSpeed.Flont);
		}
	}

	//モーション(アニメーション)が起動したか
	else if (isMotionPlaying.isUltPlay == true)
	{
		//アニメーションを流す
		AnimationPlay(AnimMontage_BossUltimate,&isMotionPlaying.isUltPlay);
		//ボスがノックバックしないようにする
		BossHPRock();
	}

	//衝撃波を出したいのでNotifyを使って制御する(UltSpawnFlagChangeにて変数の中身を変更)
	if (isShockWaveSpawnTiming == true)
	{
		//ダメージ値を代入
		BladeDamage = UltShockWaveDamage;
		//アクターのスポーン処理(ShockWaveSpawnはブループリント上で設定)
		GetWorld()->SpawnActor<AActor>(ShockWaveSpawn, GetActorTransform());
		//一度だけスポーンさせたいので切り替えておく
		isShockWaveSpawnTiming = false;
	}

	//アニメーションの最後に必殺技を出したいのでNotifyを使って制御する(UltSpawnFlagChangeにて変数の中身を変更)
	if (isUltSpawnTiming == true)
	{
		//アクターのスポーン処理(UltSpawnはブループリント上で設定)
		GetWorld()->SpawnActor<AActor>(UltSpawn, GetActorTransform());
		//一度だけスポーンさせたいので切り替えておく
		isUltSpawnTiming = false;
	}



	//250フレームたったら
	if (Timer > iAnimTime.Ultimate)
	{
		//ステートを切り替える
		OdaMoveEnum = ECPPOdaEnum::Moveflont;
		//切り替えるにあたって変数を初期化する
		TimeCounter = 0;
		isUltShotTiming = false;
	}
}
