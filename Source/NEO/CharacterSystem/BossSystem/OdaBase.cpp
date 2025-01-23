// Fill out your copyright notice in the Description page of Project Settings.


#include "OdaBase.h"
#include "NEO/GameSystem/NEOGameMode.h"
#include "NEO/AnimationSystem/CharacterAnimInstance.h"



// Sets default values
//コンストラクタ+変数の初期化
AOdaBase::AOdaBase() :
	SpawnTimer(0),						//---------------------------
	FlameCounter(0),					//タイマー系
	TimeCounter(0),						//---------------------------
	isMove(true),
	OdaMoveEnum(ECPPOdaEnum::Movie),	//列挙型
	HoldOdaMoveEnum(ECPPOdaEnum::Stay1),
	Attack1Delay(0),					//近接攻撃の待機時間
	RandomNum(-1),
	BladeDamage(0.f),
	ShockWaveDamage(10.f),
	UltShockWaveDamage(5.f),
	UltDamage(30.f),
	OdaSpeed{1.f,3.f,.5f},
	OneMoreShockWave(false),
	ChangeFlontTimer(200),
	isShockWaveSpawnTiming(false),
	isUltShotTiming(false),
	isUltShot(true),
	UltTimer(550),
	isNotAttackNow(false),
	NotAttackCount(0),
	Attack1WaitingTime(60),
	iAnimTime{ 0,0,0,0,0,0,0 },
	bStartMovieOnce(true),
	MovieMove(false),
	SwordFirstDamage(5),
	HoldDamageAdd(0),
	SwordAddDamage(5),
	EnemyOneTimeSpawn(0),
	bIsAttacked(false),
	Health(1000.f),
	MaxHealth(1000.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Assist Create
	ActionAssistComp = CreateDefaultSubobject<UActionAssistComponent>(TEXT("ActionAssist"));

}

// Called when the game starts or when spawned
void AOdaBase::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultController();

	//モーションの長さを取る(*60は秒からフレームに変換するため)
	iAnimTime.BossKnock = (int)AnimMontage_BossKnockMontage->GetPlayLength() * 60.f;
	iAnimTime.Attack1 = (int)AnimMontage_BossAttack1->GetPlayLength() * 60.f;
	iAnimTime.Attack2 = ((int)AnimMontage_BossAttack2->GetPlayLength() + 1) * 60.f;
	iAnimTime.Ultimate = ((int)AnimMontage_BossUltimate->GetPlayLength() + 1) * 60.f;
	iAnimTime.BlowAway = (int)AnimMontage_BossBlowAway->GetPlayLength() * 60.f;
	iAnimTime.Death = (int)AnimMontage_BossDeath->GetPlayLength() * 60.f;
	iAnimTime.StartMovie = (int)AnimMontage_BossStartMovie->GetPlayLength() * 60.f;


	//変数の初期化
	isMotionReset(true);

	NobunagaMovement = (GetCharacterMovement());
	PlayerChara = nullptr;

	//ディレイを一定時間か攻撃を受けたら処理を切り替える
	SpawnDelay = true;

	//ボスのHPが一気に削れないように
	isBossHPRock = true;

	
}


// Called to bind functionality to input
void AOdaBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called every frame
void AOdaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//プレイヤーがリスポーンした時に中身がなくなってしまうので更新
	AActor* Player = GetPlayer();
	//プレイヤーがなかったらこれ以降の処理をスルーする
	if (Player == nullptr)  return;

	//離れすぎたときは動かないように
	if (FVector::Dist(GetActorLocation(), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()) <= 4000.f)
	{
		//フレームごとに加算する
		FlameCounter++;

		//ムービー中なら飛ばす
		if (isMove == false)
		{
			return;
		}
		//向きをプレイヤーの方に向ける(30フレーム毎に更新)
		if (FlameCounter % 30 == 0 )
		{
			//プレイヤーの方を向く
			ToPlayerRotate();
		}

		if (isDamageHit == true)
		{
			//プレイヤーの方を向く
			ToPlayerRotate();
		}
		//距離を取る
		//X軸
		BossPosX = FVector(GetActorLocation().X, 0.f, 0.f);
		PlayerPosX = FVector(Player->GetActorLocation().X, 0.f, 0.f);

		//Y軸
		BossPosY = FVector(0.f, GetActorLocation().Y, 0.f);
		PlayerPosY = FVector(0.f, Player->GetActorLocation().Y, 0.f);

		//カウンター起動
		TimeCounter++;

		//体力が半分以下になったら
		if (Health < MaxHealth / 2.f)
		{
			//二回目を撃つためのタイマーを起動
			UltTimer++;
			if (UltTimer % 600 == 0)//600フレーム後に必殺を撃てるようにする
			{
				//必殺技を撃てるようにする
				isUltShot = false;
			}
		}


		//状態ごとに動きを切り替える
		switch (OdaMoveEnum)
		{
			//待機,動き
		case ECPPOdaEnum::Stay1:
			OdaStay1(TimeCounter);
			break;

			//待機,動き
		case ECPPOdaEnum::Stop:

			if (FlameCounter > 30)
			{
				if (HoldOdaMoveEnum == ECPPOdaEnum::Attack1)
				{
					OdaMoveEnum = ECPPOdaEnum::Attack1;
				}
				else
				{
					OdaMoveEnum = ECPPOdaEnum::Stay1;
				}
			}
			break;

		case ECPPOdaEnum::Movie:
			OdaMovie(TimeCounter);
			break;

			//正面ダッシュ
		case ECPPOdaEnum::Moveflont:
			//右向きか左向きかで正負を変える
			if (GetActorLocation().Y < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
			{
				BossMoveY(OdaSpeed.Dash);
			}
			else
			{
				BossMoveY(-OdaSpeed.Dash);
			}
			//一定時間たったら
			if (TimeCounter % 15 == 0)
			{
				//待機に戻す
				BacktoStayCase();
			}
			break;

			//背面ダッシュ
		case ECPPOdaEnum::MoveBack:
			//右向きか左向きかで正負を変える
			if (GetActorLocation().Y < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
			{
				BossMoveY(OdaSpeed.Back);
			}
			else
			{
				BossMoveY(-OdaSpeed.Back);
			}
			//一定時間たったら
			if (TimeCounter % 30 == 0)
			{
				//待機に戻す
				BacktoStayCase();
			}
			//レイを飛ばして後ろ方向の壁に当たったら
			else if (ActionAssistComp->WallCheck(-100.f))
			{
				//処理を変える
				IsWallCheck(ECPPOdaEnum::MoveBack);
			}
			break;

			//攻撃１
		case ECPPOdaEnum::Attack1:
			if (TimeCounter % Attack1WaitingTime == 0)
			{
				OdaAttack1(TimeCounter);
			}
			break;

			//攻撃２
		case ECPPOdaEnum::Attack2:
			OdaAttack2(TimeCounter);

			break;

			//必殺技
		case ECPPOdaEnum::Ultimate:
			OdaUlt(TimeCounter);
			break;

		default:
			break;
		}
	}
}
///*
// * 関数名　　　　：NomalMode()
// * 処理内容　　　：通常攻撃用関数
// * 戻り値　　　　：なし
// */
//void AOdaBase::NomalMode()
//{
//	//状態ごとに動きを切り替える
//	switch (OdaMoveEnum)
//	{
//		//待機,動き
//	case ECPPOdaEnum::Stay1:
//		OdaStay1(TimeCounter);
//		break;
//
//		//待機,動き
//	case ECPPOdaEnum::Stop:
//
//		if (FlameCounter > 30)
//		{
//			if (HoldOdaMoveEnum == ECPPOdaEnum::Attack1)
//			{
//				OdaMoveEnum = ECPPOdaEnum::Attack1;
//			}
//			else
//			{
//				OdaMoveEnum = ECPPOdaEnum::Stay1;
//			}
//		}
//		break;
//
//		//攻撃１
//	case ECPPOdaEnum::Attack1:
//		if (TimeCounter % Attack1WaitingTime == 0)
//		{
//			OdaAttack1(TimeCounter);
//		}
//		break;
//
//		//攻撃２
//	case ECPPOdaEnum::Attack2:
//		OdaAttack2(TimeCounter);
//
//		break;
//
//	}
//}
//
///*
// * 関数名　　　　：UltMode()
// * 処理内容　　　：必殺技用関数
// * 戻り値　　　　：なし
// */
//void AOdaBase::UltMode()
//{
//	//体力が半分以下になったら
//	if (Health < MaxHealth / 2.f)
//	{
//		//二回目を撃つためのタイマーを起動
//		UltTimer++;
//		if (UltTimer % 600 == 0)//600フレーム後に必殺を撃てるようにする
//		{
//			//必殺技を撃てるようにする
//			isUltShot = false;
//		}
//	}
//}


/*
 * 関数名　　　　：ToPlayerRotate()
 * 処理内容　　　：Y軸だけを見てどっち側にいるかを判定する関数
 * 戻り値　　　　：なし
 */
void AOdaBase::ToPlayerRotate()
{
	bool LookRight;

	if (isNowAttacking != true)
	{
		//ボスがプレイヤーの右にいるとき
		if (GetActorLocation().Y < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
		{
			LookRight = true;
		}
		else
		{
			LookRight = false;
		}

		ActionAssistComp->OwnerParallelToCamera(LookRight);
	}
	if (isDamageHit == true)
	{
		//ボスがプレイヤーの右にいるとき
		if (GetActorLocation().Y < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
		{
			LookRight = true;
		}
		else
		{
			LookRight = false;
		}
		//攻撃をもらった時用の変数を元に戻す
		isDamageHit = false;
		ActionAssistComp->OwnerParallelToCamera(LookRight);
	}

}


/*
 * 関数名　　　　：OdaStay1()
 * 処理内容　　　：攻撃せず待機しているときの関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaStay1(int Timer)
{
	//離れていたら
	if (FVector::Dist(BossPosY, PlayerPosY) >= 700.f)
	{
		//前か後ろに動く処理
		if (RandomNum == 0)
		{
			BossMoveY(OdaSpeed.Flont);
		}
		else if (RandomNum == 1)
		{
			BossMoveY(-OdaSpeed.Flont);
		}

	}

	//モーションを流す状態かどうか(isMotionPlayingをtrueにする)
	isMotionReset(true);

	//余りにも離れていたら前に走る
	if (FVector::Dist(BossPosY, PlayerPosY) >= 700.f)
	{
		OdaMoveEnum = ECPPOdaEnum::Moveflont;
	}

	if (FVector::Dist(BossPosX, PlayerPosX) <= 50.f)
	{

		if (isUltShot == false)
		{
			//必殺技
			OdaMoveEnum = ECPPOdaEnum::Ultimate;
			isUltShot = true;
			UltTimer = 0;

			//random固定のリセット
			if (RandomNum != -1)
			{
				RandomNum = -1;
			}
		}
		//プレイヤーとの距離の判定
		else if (FVector::Dist(BossPosY, PlayerPosY) <= 200.f)
		{
			//近接攻撃
			OdaMoveEnum = ECPPOdaEnum::Attack1;
			TimeCounter = 0;
			//もし攻撃のディレイの値が入っていなかったら
			if (Attack1Delay != 0)
			{
				Attack1Delay = 10;
			}

			//random固定のリセット
			if (RandomNum != -1)
			{
				RandomNum = -1;
			}
		}
		//離れていたら
		else if (FVector::Dist(BossPosY, PlayerPosY) >= 300.f)
		{
			//遠距離
			OdaMoveEnum = ECPPOdaEnum::Attack2;
			TimeCounter = 0;
			//random固定のリセット
			if (RandomNum != -1)
			{
				RandomNum = -1;
			}
		}
		else
		{
			//軸を合わせに行く
			OdaMove1(Timer, 60);
			Attack1Delay = 15;
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------------

	}
	else
	{
		//軸を合わせに行く
		if (GetActorLocation().X < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().X)
		{
			BossMoveX(OdaSpeed.Flont);
		}
		else
		{
			BossMoveX(-OdaSpeed.Flont);
		}

	}
}

void AOdaBase::InMovie()
{
	OdaMoveEnum = ECPPOdaEnum::Movie;
	OdaMovie(1);
}

void AOdaBase::OdaMovie(int Timer)
{
	if (bStartMovieOnce)
	{
		//最初の向きが正面を向くように修正
		ToPlayerRotate();
		//ムービー用アニメーションを流す
		PlayAnimMontage(AnimMontage_BossStartMovie);
		//一度だけながす
		bStartMovieOnce = false;
	}
	//ムービー終了後待機に戻す
	if (Timer > iAnimTime.StartMovie)
	{
		OdaMoveEnum = ECPPOdaEnum::Stay1;
	}
	if (Timer % 260 == 0)
	{
		MovieMove = true;
	}
	if (MovieMove)
	{
		//動かす
		if (GetActorLocation().Y < UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation().Y)
		{
			SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y + 3.5f, GetActorLocation().Z), true);
		}
	}
}

/*
 * 関数名　　　　：OdaMove1()
 * 処理内容　　　：軸を合わせるなど動きについての関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaMove1(int DeltaTime, int StopTimer)
{
	if (isAttack1Waiting)
	{
		isAttack1Waiting = false;
	}

	//軸を合わせに行く
	//randomの中に値が入っているか(-1が入っていない状態)
	if (RandomNum == -1)
	{
		//randomで0か1を入れる
		RandomNum = FMath::RandRange(0, 1);
	}

	//前か後ろに動く処理
	if (RandomNum == 0)
	{
		BossMoveY(OdaSpeed.Flont);
	}
	else if (RandomNum == 1)
	{
		BossMoveY(-OdaSpeed.Flont);
	}

}
/*
 * 関数名　　　　：IsWallCheck()
 * 処理内容　　　：下がらせすぎないようにする関数
 * 引数  　　　　：現在の状態(攻撃等)
 * 戻り値　　　　：なし
 */
void AOdaBase::IsWallCheck(ECPPOdaEnum BossState) {

	//遠距離攻撃後なら
	if (BossState == ECPPOdaEnum::Moveflont)
	{
		//即時待機に移る
		BacktoStayCase();
	}
	//必殺技なら
	if (BossState == ECPPOdaEnum::Ultimate) {
		isUltShotTiming = true;
	}
	//ノックバック時なら
	if (BossState == ECPPOdaEnum::MoveBack)
	{
		//即時待機に移る
		BacktoStayCase();
	}
}

/*
 * 関数名　　　　：OdaAttack1()
 * 処理内容　　　：近距離攻撃をするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaAttack1(int Timer) {
	//UKismetSystemLibrary::PrintString(this, "Attack1", true, true, FColor::Cyan, 2.f, TEXT("None"));

	if (Attack1Delay == 0)
	{
		//0だと割り切れないので1を代入しておく
		Attack1Delay = 1;
	}

	//設定した時間を超えたか
	if (Timer % Attack1Delay == 0)
	{
		//モーション(アニメーション)が起動したか
		if (isMotionPlaying.isAttack1Play == true)
		{
			//アニメーションを流す
			AnimationPlay(AnimMontage_BossAttack1,&isMotionPlaying.isAttack1Play);
			//ボスがノックバックしないようにする
			isBossHPRock = false;
			//ディレイをリセットする
			Attack1Delay = 0;
			//振り向かないようにする
			isNowAttacking = true;
		}
	}

	//アニメーションが終了したら
	if (Timer > iAnimTime.Attack1)
	{
		//振り向けるようにする
		isNowAttacking = false;
		isDamageHit = false;
		//ステートを切り替える
		BacktoStayCase();
		//リセット
		NotAttackCount = 0;
		//ノックバック中に攻撃モーションに入るとHPロックが作動し続けてしまうのでここで切り替える
		if (isHPLock == true)
		{
			HPLock();
		}
	}
}


/*
 * 関数名　　　　：OdaAttack2()
 * 処理内容　　　：遠距離攻撃をするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaAttack2(int Timer) {
	//UKismetSystemLibrary::PrintString(this, "Attack2", true, true, FColor::Cyan, 2.f, TEXT("None"));

	//モーション(アニメーション)が起動したか
	if (isMotionPlaying.isAttack2Play == true)
	{
		//ダメージ値を代入
		BladeDamage = ShockWaveDamage;
		//アニメーションを流す
		AnimationPlay(AnimMontage_BossAttack2,&isMotionPlaying.isAttack2Play);
		//振り向かないようにする
		isNowAttacking = true;
	}


	//アニメーションの最後に衝撃波を出したいのでNotifyを使って制御する(ShockWaveSpawnFlagChangeにて変数の中身を変更)
	if (isShockWaveSpawnTiming == true)
	{
		//アクターのスポーン処理(ShockWaveSpawnはブループリント上で設定)
		GetWorld()->SpawnActor<AActor>(ShockWaveSpawn, GetActorTransform());
		//一度だけスポーンさせたいので切り替えておく
		isShockWaveSpawnTiming = false;
	}

	//アニメーションが終了したら
	if (Timer > iAnimTime.Attack2)
	{
		//振り向けるようにする
		isNowAttacking = false;

		//前ダッシュしてみる
		OdaMoveEnum = ECPPOdaEnum::Moveflont;

		//切り替えるにあたって変数を初期化する
		TimeCounter = 0;
		//リセット
		NotAttackCount = 0;
		//ノックバック中に攻撃モーションに入るとHPロックが作動し続けてしまうのでここで切り替える
		if (isHPLock == true)
		{
			HPLock();
		}
	}
}
/*
 * 関数名　　　　：OdaUlt()
 * 処理内容　　　：必殺技攻撃をするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::OdaUlt(int Timer)
{

	if (Timer % 30 == 0 && isUltShotTiming != true)
	{
		isUltShotTiming = true;
	}
	//軸を合わせに行く
	if (isUltShotTiming != true)
	{
		//振り向かないようにする
		isNowAttacking = true;
		//レイを飛ばして後ろ方向の壁に当たったら
		if (ActionAssistComp->WallCheck(-100.f))
		{
			//処理を変える
			IsWallCheck(ECPPOdaEnum::Ultimate);
		}
		//後退
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



	//アニメーションが終了したら
	if (Timer > iAnimTime.Ultimate)
	{
		//振り向けるようにする
		isNowAttacking = false;

		//ステートを切り替える
		OdaMoveEnum = ECPPOdaEnum::Moveflont;
		//待機モーションに切り替える
		Cast < UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCharacterState(ECharacterState::State_Idle);


		//切り替えるにあたって変数を初期化する
		TimeCounter = 0;
		isUltShotTiming = false;
	}
}
/*
 * 関数名　　　　：AttackFlagChange()
 * 処理内容　　　：AnimNotifyにて変更、攻撃のフラグのon,offについての関数
 * 戻り値　　　　：なし
 */
void AOdaBase::AttackFlagChange()
{
	//反転させる
	IsAttackNow = !IsAttackNow;
}
/*
* 関数名　　　　：isMotionReset()
* 処理内容　　　：モーションを起動下かどうかの変数をリセットする為の関数
* 戻り値　　　　：なし
*/
void AOdaBase::isMotionReset(bool _isPlaying)
{
	isMotionPlaying.isAttack1Play = _isPlaying;
	isMotionPlaying.isAttack1Play = _isPlaying;
	isMotionPlaying.isAttack2Play = _isPlaying;
	isMotionPlaying.isUltPlay = _isPlaying;
	isMotionPlaying.isDeathPlay = _isPlaying;
	isMotionPlaying.isMoviePlay = _isPlaying;
}


/*
* 関数名　　　　：setRootMotion()
* 処理内容　　　：ルートモーション(疑似)をするための関数
* 戻り値　　　　：なし
*/
void AOdaBase::setRootMotionMove(float speed)
{
	if (GetActorRotation() == FRotator(0.f,90.f,0.f))
	{
		BossMoveY(speed);
	}
	else
	{
		BossMoveY(-speed);
	}
}

/*
 * 関数名　　　　：ShockWaveSpawnFlagChange()
 * 処理内容　　　：衝撃波をスポーンさせるための変数を切り替える為の関数
 * 戻り値　　　　：なし
 */
void AOdaBase::ShockWaveSpawnFlagChange()
{
	//衝撃波を出現させる為の変数をtrueに変える
	isShockWaveSpawnTiming = true;
	//もしIsAttackNowがtrueになっていたら
	if (IsAttackNow == true)
	{
		//falseにする
		IsAttackNow = false;
	}
}

/*
 * 関数名　　　　：UltSpawnFlagChange()
 * 処理内容　　　：必殺技をスポーンさせるための変数を切り替える為の関数
 * 戻り値　　　　：なし
 */
void AOdaBase::UltSpawnFlagChange(TSubclassOf<AActor>_UltSpawn)
{
	//引数の中に値がはいっていたら
	if(UltSpawn != NULL)
	{
		UltSpawn = _UltSpawn;
	}
	//必殺技を出現させる為の変数をtrueに変える
	isUltSpawnTiming = true;
	//もしIsAttackNowがtrueになっていたら
	if (IsAttackNow == true)
	{
		//falseにしてみる
		IsAttackNow = false;
	}
}

/*
 * 関数名　　　　：ApplyDamage()
 * 処理内容　　　：ダメージを受けた時の処理
 * 戻り値　　　　：なし
 */
void AOdaBase::TakedDamage(float Damage , bool _bLastAttacked)
{
	if (FlameCounter >= 100)
	{
		FlameCounter = 0;
	}
	//現在の状態を保存
	HoldOdaMoveEnum = OdaMoveEnum;
	//動きを止める
	OdaMoveEnum = ECPPOdaEnum::Stop;

	if (isHPLock != true)
	{
		//ボスのスポーン時待機時間中に攻撃を受けた時
		if (SpawnDelay == true)
		{
			//フラグを切り替える
			SpawnDelay = false;
			//攻撃のディレイをセット
			Attack1Delay = 10;
		}

		if (isMove)
		{
			Health -= Damage;
			if (isBossHPRock)
			{
				if (OdaMoveEnum == ECPPOdaEnum::Stay1)
				{
					if (FMath::RandRange(0, 100) <= 25)
					{


						//ノックバックアニメーションを流す
						PlayAnimMontage(AnimMontage_BossBlowAway);

						//背面ダッシュに切り替えて仕切り直す
						OdaMoveEnum = ECPPOdaEnum::MoveBack;

						//アニメーション終了後HPロックを解除してStayに戻す
						FTimerManager& TimerManager = GetWorld()->GetTimerManager();
						TimerManager.SetTimer(TimerHandle_KnockbackAnimationFinish, this, &AOdaBase::BossHPRock, iAnimTime.BlowAway - 1, false);
						TimerManager.SetTimer(TimerHandle_KnockbackAnimationFinish, this, &AOdaBase::BacktoStayCase, iAnimTime.BlowAway, false);
					}


					else
					{
						//ノックバックのアニメーションを流す
						PlayAnimMontage(AnimMontage_BossKnockMontage);
						// アニメーション終了後Stayに戻す
						FTimerManager& TimerManager = GetWorld()->GetTimerManager();
						TimerManager.SetTimer(TimerHandle_KnockbackAnimationFinish, this, &AOdaBase::BossHPRock, iAnimTime.BlowAway - 1, false);
						TimerManager.SetTimer(TimerHandle_KnockbackAnimationFinish, this, &AOdaBase::BacktoStayCase, iAnimTime.BlowAway, false);
					}
				}
			}
		}

		//HPが0になったら
		if (Health <= 0.f)
		{
			//動けるかどうかの変数をfalseにしておく
			if (isMove == true)
			{
				//フラグを使ってダメージを受けるアニメーションを動かさないようにする
				isMove = false;
				//一応アニメーションを止めておく
				this->StopAnimMontage();

				//ゲーム全体の速度を遅くする
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), .2f);

				//アニメーションを流す
				AnimationPlay(AnimMontage_BossDeath, &isMotionPlaying.isDeathPlay);

				//アニメーションの長さを判定してその時間が過ぎたら死亡処理の関数に飛ばせる処理
				FTimerManager& TimerManager2 = GetWorld()->GetTimerManager();
				TimerManager2.SetTimer(TimerHandle_DeathToGameOver, this, &AOdaBase::Death, iAnimTime.Death, false);
			}
		}
		for (int i = 0; i < Enemy.Num(); i++)
		{
			if (Health <= Enemy[i].BossHPTiming && Enemy[i].isSpawn)
			{
				CallEnemy(i);
			}
		}
	}
}

/*
 * 関数名　　　　：CallEnemy()
 * 処理内容　　　：敵を呼ぶ処理
 * 引数１	　　 ：要素番号
 * 戻り値　　　　：なし
 */
void AOdaBase::CallEnemy(int _element)
{
	//TSubclassOf<class AActor> Enemy = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous(); // 上記で設定したパスに該当するクラスを取得
	EnemyActor.Add(GetWorld()->SpawnActor<AActor>(Enemy[_element].Enemy)); // スポーン処理
	EnemyActor.Last()->SetActorLocation(Enemy[_element].SpawnPoint); // 確認しやすいように座標を設定
	Enemy[_element].isSpawn = false;									//もう一度処理されないように切り替える
}

/*
 * 関数名　　　　：WorldTimeReturn()
 * 処理内容　　　：ずっと遅いままだとストレスがたまるので元の速度に戻す(アニメーション通知で管理)
 * 戻り値　　　　：なし
 */
void AOdaBase::WorldTimeReturn()
{
	//ついでに敵も消す
	for (int i = 0; i < EnemyActor.Num(); i++)
	{
		if (EnemyActor[i] != NULL)
		{
			EnemyActor[i]->Destroy();
			EnemyActor[i] = NULL;
		}
	}
	//時間を戻す
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

/*
 * 関数名　　　　：BossHPRock()
 * 処理内容　　　：ボスをノックバックさせないようにする処理
 * 戻り値　　　　：なし
 */
void AOdaBase::BossHPRock()
{
	isBossHPRock = !isBossHPRock;
}



/*
 * 関数名　　　　：BacktoStayCase()
 * 処理内容　　　：ボスの行動を待機に戻す処理
 * 戻り値　　　　：なし
 */
void AOdaBase::BacktoStayCase()
{
	//待機に戻す
	OdaMoveEnum = ECPPOdaEnum::Stay1;
	//切り替えるにあたって変数を初期化する
	TimeCounter = 0;
	//プレイヤーの方を向く
	ToPlayerRotate();
	//もしHPロックが外れていたら
	if (isBossHPRock != true)
	{
		//ダメージを受けた時ノックバックするように
		BossHPRock();
	}
	if (!bStartMovieOnce)
	{
		bStartMovieOnce = true;
	}

	//待機モーションに切り替える
	Cast < UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCharacterState(ECharacterState::State_Idle);
}
void AOdaBase::AnimationPlay(class UAnimMontage* AnimMontage_AnimPlay ,bool* _isMotionPlaying)
{
	//アニメーションを流す(今は仮)
	PlayAnimMontage(AnimMontage_AnimPlay);
	//一度だけ流したいのでフラグを切り替える
	if (_isMotionPlaying != nullptr)
	{
		*_isMotionPlaying = false;
	}
}


void AOdaBase::BossKnockback()
{
	//
	if (!isBossHPRock)
	{
		isDamageHit = true;
		OdaMoveEnum = ECPPOdaEnum::MoveBack;
		if (isMove)
		{
			PlayAnimMontage(AnimMontage_BossBlowAway);
		}
	}
}

/*
 * 関数名　　　　：HPLock()
 * 処理内容　　　：HPロック用のスイッチを切り替える処理(プレイヤー側)
 * 戻り値　　　　：なし
 */
void AOdaBase::HPLock()
{
	//HPロック用のスイッチを切り替える
	isHPLock = !isHPLock;
}

//動き関連

/*
 * 関数名　　　　：BossMoveX()
 * 処理内容　　　：正面方向に歩く処理
 * 戻り値　　　　：なし
 */
void AOdaBase::BossMoveX(float Speed)
{
	//上下方向に歩く処理
	SetActorLocation(FVector(GetActorLocation().X + Speed, GetActorLocation().Y, GetActorLocation().Z));
	Cast < UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCharacterState(ECharacterState::State_Move);

}

/*
 * 関数名　　　　：BossMoveYY()
 * 処理内容　　　：正面方向に歩く処理
 * 戻り値　　　　：なし
 */
void AOdaBase::BossMoveY(float Speed)
{
	//正面方向に歩く処理
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y + Speed, GetActorLocation().Z));

	Cast < UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCharacterState(ECharacterState::State_Move);
}

/*
 * 関数名　　　　：GetPlayer()
 * 処理内容　　　：プレイヤーを取得する処理
 * 戻り値　　　　：AActor(プレイヤーの情報)
 */
AActor* AOdaBase::GetPlayer()
{
	PlayerChara = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return PlayerChara;
}

/*
 * 関数名　　　　：toPlayerAttacked()
 * 処理内容　　　：プレイヤーに対してのHPロックするための関数
 * 戻り値　　　　：AActor(プレイヤーの情報)
 */
void AOdaBase::toPlayerAttacked()
{
	if (bIsAttacked)
	{
		bIsAttacked = false;
	}
}

/*
 * 関数名　　　　：Is2Combo()
 * 処理内容　　　：遠距離攻撃のコンボをするための関数
 * 戻り値　　　　：なし
 */
void AOdaBase::Is2Combo()
{
	if (Combo2Counter == 0)
	{
		//HPが2/3以下じゃなかったら以下の処理
		if (Health > MaxHealth * (2.f / 3.f))
		{
			//アニメーションを止める
			GetMesh()->GetAnimInstance()->StopSlotAnimation();
			//変数をリセット
			Combo2Counter = 0;

			Cast < UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCharacterState(ECharacterState::State_Idle);

		}
		else
		{
			Combo2Counter++;
		}
	}
	else if (Combo2Counter == 1)
	{
		//HPが1/3以下じゃなかったら以下の処理
		if (Health > MaxHealth * (1.f / 3.f))
		{
			//アニメーションを止める
			GetMesh()->GetAnimInstance()->StopSlotAnimation();
			//変数をリセット
			Combo2Counter = 0;
		}
		else
		{
			Combo2Counter++;
		}
	}
	else if (Combo2Counter)
	{
		//変数をリセット
		Combo2Counter = 0;
	}
}
/*
 * 関数名　　　　：LastAttack()
 * 処理内容　　　：プレイヤーをノックバックさせるために最後の攻撃かどうかをとる関数
 * 戻り値　　　　：bool(最後の攻撃ならtrueをかえす)
 */
bool AOdaBase::LastAttack()
{
	if (isMove)
	{
		if (Combo1Counter >= 3)
		{
			return true;
		}

		else if (Combo2Counter == 2)
		{
			return true;
		}
	}
	return false;
}



/*
 * 関数名　　　　：Death()
 * 処理内容　　　：体力が0以下になったときに動く関数
 * 戻り値　　　　：なし
 */
void AOdaBase::Death()
{
	//ゲームモードにてこのアクタを消す処理
	ANEOGameMode* GameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		if (this != NULL)
		{
			//現在エラーで落ちるので普通に消す
			//ゲームモードを用いて消す
			GameMode->DestroyEnemy(this, IsAreaEnemy);
		}
	}
}

// 攻撃が当たった時の処理
void AOdaBase::Attack_Sword(TTuple<TArray<AActor*>, TArray<FVector>> HitActorAndLocation)
{
	//今当たっているアクターが取れる(AActor*)
	TArray<AActor*> DamageInfo = HitActorAndLocation.Get<0>();

	//当たった場所をとれる
	TArray<FVector> EffectPoint = HitActorAndLocation.Get<1>();

	for (AActor* ActorArray : DamageInfo)
	{
		if (ActorArray != nullptr)
		{
			if (ActorArray->ActorHasTag("Player"))
			{
				//プレイヤーにダメージを与える(引数１：ダメージ値、引数2：最後の攻撃かどうか(ノックバックさせるため))
				Cast<ACharacterBase>(ActorArray)->TakedDamage(10.f,false);
				break;
			}
		}
	}

}


/*
 * 関数名　　　　：CheckOverlap()
 * 処理内容　　　：ボックスコリジョンに当たっているかチェック
 * 戻り値　　　　：なし
 */
void AOdaBase::CheckOverlap()
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FVector Start = BoxComp->GetComponentLocation();
	FVector End = Start;// + GetActorForwardVector() * 100.0f;
	FQuat Rot = BoxComp->GetComponentQuat();			// 
	FCollisionShape CollisionBox = FCollisionShape::MakeBox(BoxComp->GetUnscaledBoxExtent());

	bool isHit = GetWorld()->SweepMultiByChannel(HitResults, Start, End, Rot, ECollisionChannel::ECC_GameTraceChannel1, CollisionBox, CollisionParams);

	if (isHit != true) { return; }

	if (false) {
		DrawDebugCapsule(GetWorld(), (Start + End) / 2,
			CollisionBox.GetCapsuleHalfHeight(),
			CollisionBox.GetCapsuleRadius(), Rot, FColor::Red, true, -1.0f, 0, 1.0f);
	}
}

/*
 * 関数名　　　　：PlayerOnOverlap()
 * 処理内容　　　：プレイヤーが当たったらの処理
 * 戻り値　　　　：なし
 */
void AOdaBase::PlayerOnOverlap(/*FHitResult& _HitResult*/)
{
	//プレイヤーがHPをロックしたらこの処理を通る
	if (bIsAttacked) {
		return;
	}

	//Weapon->SetCollision();

	//リセット
	bIsAttacked = true;
}
