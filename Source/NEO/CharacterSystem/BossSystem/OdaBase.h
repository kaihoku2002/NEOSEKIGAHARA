// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPOdaEnum.h"
#include "GameFramework/CharacterMovementComponent.h"			//キャラクタームーブメントを使うために必要なヘッダー
#include "Kismet/GameplayStatics.h"								//getplayerpawn等を使うために必要なヘッダー
#include "Kismet/KismetSystemLibrary.h"							//printStringを使うために必要なヘッダー
#include "Animation/AnimMontage.h"								//アニメーションを流すために必要なヘッダー
#include "Math/UnrealMathUtility.h"								//RandRangeを使うために必要なヘッダー
#include "Math/Vector.h"										//Dist(距離)を使うために必要なヘッダー
#include "Components/BoxComponent.h"							//ボックスコンポーネントを使うため
#include "Components/CapsuleComponent.h"						//カプセルコンポーネントを使うため
#include "Templates/SubclassOf.h"								//TSubclassOfを使うために必要なヘッダー
#include "NEO/CharacterSystem/PlayerSystem/PlayerCharacter.h"	//プレイヤーキャラを取得するために必要なヘッダー
#include "NiagaraComponent.h"									//ナイアガラエフェクトを呼ぶために必要なヘッダー
#include "NiagaraFunctionLibrary.h"								//ナイアガラエフェクトを呼ぶために必要なヘッダー
#include "NEO/CharacterSystem/ActionAssistComponent.h"
#include "NEO/CharacterSystem/CharacterBase.h"

#include "OdaBase.generated.h"

class AWeaponBase;

USTRUCT(BlueprintType)
struct FEnemyInfo {
	GENERATED_BODY()
		UPROPERTY(EditAnywhere)
		TSubclassOf<class AActor> Enemy;

	UPROPERTY(EditAnywhere)
		float BossHPTiming;

	UPROPERTY(EditAnywhere)
		FVector SpawnPoint;

	UPROPERTY(EditAnywhere)
		bool isSpawn;
};

//通常の移動速度
USTRUCT(BlueprintType)
struct FBossSpeed {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Flont;

	UPROPERTY(EditAnywhere)
	float Dash;

	UPROPERTY(EditAnywhere)
	float Back;
};

UCLASS()
class NEO_API AOdaBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOdaBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//関数-------------------------------------------------------------------------------------------------------------------

	//ボスのステートでの処理----------------
	void OdaStay1(int Timer);

	void OdaMovie(int Timer);

	void OdaAttack1(int Timer);

	void OdaAttack2(int Timer);

	void OdaUlt(int Timer);
	//------------------------------------

	//ボスの動きの処理
	void OdaMove1(int DeltaTime, int StopTimer);

	void IsWallCheck(ECPPOdaEnum BossState);

	//変数---------------------------------------------------------------------------------------------

	//スポーン時処理--------------------------------------------------------------------------
	//スポーン中の止まるときかどうか
	UPROPERTY()
		bool SpawnDelay;

	//スポーン中止まる時間
	UPROPERTY()
		int SpawnTimer;
	//----------------------------------------------------------------------------------------


	//カウンター------------------------------------------------------------
	//時間の取得
	int FlameCounter;

	//int型のカウンター
	int TimeCounter;
	//---------------------------------------------------------------------

	//ボスが動いていいかどうか
	UPROPERTY(BlueprintReadOnly)
		bool isMove;

	//ボスとプレイヤーとの距離----------------------------
	//X軸
	//ボス
	FVector BossPosX;
	//プレイヤー
	FVector PlayerPosX;
	//Y軸
	//ボス
	FVector BossPosY;
	//プレイヤー
	FVector PlayerPosY;
	//---------------------------------------------------

public:
	//列挙型
	UPROPERTY(EditAnywhere)
		ECPPOdaEnum OdaMoveEnum;

protected:

	ECPPOdaEnum HoldOdaMoveEnum;


	//近接攻撃が早すぎてプレイヤーがよけれないので少し遅延させる
	UPROPERTY()
		int Attack1Delay;

	//randomの値を入れる為の変数
	UPROPERTY()
		int RandomNum;

	//攻撃中振り向けないように
	bool isNowAttacking = false;

public:

	//刀の情報を取得
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sword", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* swordConp;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//コンポーネント-----------------------------------------------------------------------------------------------------
	//シーンコンポーネント
	UPROPERTY()
		USceneComponent* Parent;

	//プレイヤーキャラクター
	UPROPERTY()
		AActor* PlayerChara;

	//剣のコリジョン
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BoxComp", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxComp;

	//キャラクタームーブメント
	UPROPERTY()
		UCharacterMovementComponent* NobunagaMovement;
	//--------------------------------------------------------------------------------------------------------------------

	UPROPERTY()
		ECPPOdaState isModeNow;
	//UFUNCTION()
	//void NomalMode();
	//UFUNCTION()
	//void UltMode();


	//ダメージ処理-----------------------------------------------------
	UFUNCTION()
		virtual void TakedDamage(float Damage , bool _bLastAttacked = false);

	// 攻撃が当たった時の処理
	virtual void Attack_Sword(TTuple<TArray<AActor*>, TArray<FVector>> HitActorAndLocation);	// 刀の処理


	UFUNCTION()
		void WorldTimeReturn();

	//ダメージタイプクラス
	UPROPERTY()
		TSubclassOf < class UDamageType > DamageTypeClass;

	//衝撃波のダメージを入れるための変数
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
		float BladeDamage;

	//通常の衝撃波のダメージ
	UPROPERTY(EditAnywhere, Category = "Damage")
		float ShockWaveDamage;

	//必殺技時の衝撃波のダメージ
	UPROPERTY(EditAnywhere, Category = "Damage")
		float UltShockWaveDamage;

	//必殺技のダメージ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float UltDamage;

	//ダメージをもらったかどうか
	UPROPERTY()
		bool isDamageHit = false;

	//----------------------------------------------------------------


	//ルートモーション(疑似)の作成
	UFUNCTION()
		void setRootMotionMove(float speed);


	//速度----------------------------------
	UPROPERTY(EditAnywhere)
		FBossSpeed OdaSpeed;
	//--------------------------------------


	//衝撃波-----------------------------------------------------------------------------
	//もう一回衝撃波を出すための変数
	UPROPERTY()
		bool OneMoreShockWave;

	//行動変更時間設定
	UPROPERTY(EditAnywhere)
		int ChangeFlontTimer;

	//モーションを一回だけ流すための変数
	struct MotionPlaying
	{
		bool isKnockbackPlay;
		bool isAttack1Play;
		bool isAttack2Play;
		bool isUltPlay;
		bool isDeathPlay;
		bool isMoviePlay;
	};
	MotionPlaying isMotionPlaying;
	//初期化するようの関数
	void isMotionReset(bool _isPlaying);

	//衝撃波の出現タイミングの調整
	UPROPERTY()
		bool isShockWaveSpawnTiming;

	//衝撃波の出現タイミングの調整するための関数
	UFUNCTION()
		void ShockWaveSpawnFlagChange();

	//衝撃波を出現させる為の変数
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor>ShockWaveSpawn;
	//-----------------------------------------------------------------------------------

	//必殺技---------------------------------------------------------------------------
	//必殺技の出現タイミングの調整
	UPROPERTY()
		bool isUltSpawnTiming;

	//必殺技を打つタイミングの調整
	UPROPERTY(BlueprintReadOnly)
		bool isUltShotTiming;

	//必殺技の出現タイミングの調整するための関数
	UFUNCTION()
		void UltSpawnFlagChange(TSubclassOf<AActor>_UltSpawn);

	//必殺技を出現させる為の変数
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor>UltSpawn;

	//必殺技は一回だけ出したいのでフラグで管理
	UPROPERTY()
		bool isUltShot;

	//必殺技をもう一回打つためのカウンター
	UPROPERTY()
		int UltTimer;

	//------------------------------------------------------------------------------

	//攻撃のフラグ
	UPROPERTY(BlueprintReadOnly)
		bool IsAttackNow;

	//攻撃のフラグを切り替える為の関数
	UFUNCTION()
		void AttackFlagChange();

	//攻撃された回数が一定数超えたら近接攻撃をぱなす為の変数
	UPROPERTY()
		bool isNotAttackNow;

	//一定数の計測
	UPROPERTY()
		int NotAttackCount;

	//ヒットエフェクト変数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
		UNiagaraSystem* HitParticles;

	//HPが連続で減らないようにロック
	UPROPERTY()
		bool isHPLock;

	//HPが連続で減らないようにロックするための関数
	UFUNCTION()
		void HPLock();

	//プレイヤーの方向に向ける処理
	UFUNCTION()
		void ToPlayerRotate();

	//遠距離攻撃のコンボ
	UFUNCTION()
		void Is2Combo();

	//コンボしたかどうかで待機時間が変わるのでタイマーを用意
	UPROPERTY()
		int Attack1WaitTimer;

	//近接の待機時間
	UPROPERTY(EditAnywhere)
		int Attack1WaitingTime;

	//待機するかどうか
	bool isAttack1Waiting;

	//近接コンボが何段目か
	UPROPERTY()
		int Combo1Counter;

	//遠距離コンボが何段目か
	UPROPERTY()
		int Combo2Counter;

	//ダメージ加算した値を保存しておく変数
	float HoldDamageAdd;

	//プレイヤーをノックバックさせるために最後の攻撃かどうかをとる関数
	UFUNCTION(BlueprintCallable)
		bool LastAttack();

	UPROPERTY()
		bool isBossHPRock;

	UFUNCTION()
		void BossHPRock();

	//列挙型をStayに戻す為の関数
	UFUNCTION()
		void BacktoStayCase();


	//モーション--------------------------------------------------------------------------------------------------
	//のけぞるモーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AniMontage", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AnimMontage_BossKnockMontage;		//メモ：変数を作る際AnimMontage_は必須らしい

	//攻撃する仮モーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AniMontage", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AnimMontage_BossAttack1;

	//衝撃波攻撃する仮モーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AniMontage", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AnimMontage_BossAttack2;

	//衝撃波攻撃する仮モーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AniMontage", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AnimMontage_BossUltimate;

	//ふっとぶモーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AniMontage", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AnimMontage_BossBlowAway;

	//死亡モーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AniMontage", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AnimMontage_BossDeath;

	//ムービー中モーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AniMontage", meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* AnimMontage_BossStartMovie;

	//-------------------------------------------------------------------------------------------------------------

	//アニメーションの長さを取る
	struct AnimTime {
		int BossKnock;
		int Attack1;
		int Attack2;
		int Ultimate;
		int BlowAway;
		int Death;
		int StartMovie;
	};

	AnimTime iAnimTime;

	//スタートムービーアニメーションを一回だけ流す為の変数
	bool bStartMovieOnce;

	//ムービー中に動かすフラグを管理する変数
	bool MovieMove;

	//スタートムービーを流すかどうかの関数
	void InMovie();

	//アニメーションさせる為の関数
	void AnimationPlay(class UAnimMontage* AnimMontage_AnimPlay, bool* _isMotionPlay);

	//ノックバックしてふっとぶ関数
	UFUNCTION()
		void BossKnockback();

	FTimerHandle TimerHandle_DeathToGameOver;

	FTimerHandle TimerHandle_KnockbackAnimationFinish;
	//ムービー用アニメーション終了を見るために必要なハンドル
	FTimerHandle TimerHandle_MovieAnimationFinish;

	//上下移動
	UFUNCTION()
		void BossMoveX(float Speed);

	//前方移動
	UFUNCTION()
		void BossMoveY(float Speed);

	//ダメージ値
	UPROPERTY(EditAnywhere, Category = "Damage")
		float SwordFirstDamage;

	UPROPERTY(EditAnywhere, Category = "Damage")
		float SwordAddDamage;

	//雑魚敵を呼ぶ処理
	UFUNCTION(BlueprintCallable)
		void CallEnemy(int _element);

	//雑魚敵を呼ぶ処理を一回だけ動かす為の変数
	int EnemyOneTimeSpawn;

	//銃持った敵のパス
	FString GunPath = "/Game/0102/Enemy/Gunman/MyGunMan3.MyGunMan3_C"; // /Content 以下のパスが /Game 以下のパスに置き換わる

	//やり持った敵のパス
	FString LancePath = "/Game/0102/Enemy/Lancer/BP_LancerTag3.BP_LancerTag3_C"; // /Content 以下のパスが /Game 以下のパスに置き換わる

	//剣持った敵のパス
	FString SwordPath = "/Game/0102/Enemy/Soldier/BP_SoldierTag3.BP_SoldierTag3_C"; // /Content 以下のパスが /Game 以下のパスに置き換わる


	//エネミーの情報
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	TArray<FEnemyInfo> Enemy;
	//敵を出現、削除するための配列
	TArray<AActor*> EnemyActor;

	//void EnemySpawnPoint(int _PointCount);

		//ボックスコンポーネントのオーバーラップ処理
		UFUNCTION()
		void CheckOverlap();

	//プレイヤー----------------------------------------------------------------------
	//プレイヤーに当たったら
	UFUNCTION()
		void PlayerOnOverlap(/*FHitResult& _HitResult*/);

	//プレイヤーに対してのHPロック
	UPROPERTY()
		bool bIsAttacked;

	//プレイヤーに対してのHPロックするための関数
	UFUNCTION()
		void toPlayerAttacked();
	//-----------------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//死亡処理
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
		bool IsAreaEnemy = false;

	void Death();

	UFUNCTION(BlueprintCallable)
		float GetHP()const { return Health; }

	//体力
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Health;

	UPROPERTY(EditAnywhere)
		float MaxHealth;

protected:
	AActor* GetPlayer();
};
