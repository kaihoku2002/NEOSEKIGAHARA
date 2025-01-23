
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "NEO/WeaponSystem/WeaponBase.h"

#include "CharacterBase.generated.h"



//-----------------キャラクターのステータス-------------------------------------------------
USTRUCT(BlueprintType)
struct FCharacterStatus
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 HP;								// ヒットポイント

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MoveSpeed;						// 移動速度

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float JumpHeight;						// ジャンプ力

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float ComboDamageFactor;				// コンボ時のダメージ倍率


	// 初期化関数
	FCharacterStatus InitParam(int32 _hp,float _moveSpeed, float _jumpHeight, float _comboDamageFactor)
	{
		FCharacterStatus DefaultStatus;

		DefaultStatus.HP = _hp;
		DefaultStatus.MoveSpeed = _moveSpeed;
		DefaultStatus.JumpHeight = _jumpHeight;
		DefaultStatus.ComboDamageFactor = _comboDamageFactor;
		return DefaultStatus;
	}
};
//----------------------------------------------------------------------------------------

UCLASS()
class NEO_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 攻撃の当たり判定を発生させる
	void SetCollision();

	// 持っている武器の種類取得
	EWeaponType GetWeaponType()const { return WeaponType; }

	// 被ダメージ処理
	virtual void TakedDamage(float _damge, bool _bLastAttack = false) { return; }

	// 前方後方になにかあるかチェック
	bool WallCheck(float _lineLength);

protected:

	// 移動処理
	virtual void Move() { return; }
	virtual void Move(const FInputActionValue& _value) { return; }

	// ジャンプ処理
	virtual void Jump() { return; }

	// 攻撃が当たった時の処理
	virtual void Attack_Sword(TTuple<TArray<AActor*>,TArray<FVector>> HitActorAndLocation) { return; }	// 刀の処理
	virtual void Attack_Lance(TTuple<TArray<AActor*>,TArray<FVector>> HitActorAndLocation) { return; }	// 槍の処理	
	virtual void Attack_Gun();																			// 銃の処理

	// 武器を取得
	bool AttachWeapon(AWeaponBase* _newWeapon, FName _socketName = "None", bool _bDestroy = true);

	// 武器を離す
	void DetachWeapon(bool _bDestroy = true);

	// 武器を持ち替える
	bool ChangeWeapon(TSubclassOf<AWeaponBase> _newWeaponClass, FName _socketName = "None",bool _bDestroy = true);

	// ヒットストップ処理
	void HitStop(float _speedDuringHitStop, float _stopTime);

	// 武器取得
	AWeaponBase* GetWeapon()const { return Weapon; }

	// ステータス設定
	void InitStatus(int32 _hp, float _moveSpeed, float _jumpHeight, float _comboDamageFactor);

	// ステータス取得
	FCharacterStatus GetStatus()const { return Status; }

	// 武器の攻撃力取得
	int32 GetWeaponDamage()const { return Weapon->GetDamage(); }

	// ゲーム開始時に武器をスポーン
	void SpawnWeapon_BeginPlay();

	// 指定したパスのアニメーションアセットを返す
	UAnimMontage* GetAnimationAsset(TCHAR* _animAssetPath);

private:

	// 当たったオブジェクトをActorに変換してヒットした位置も返す
	TTuple<TArray<AActor*>,TArray<FVector>> HitObjToActor(TArray<FHitResult> _hitResults);

	// ヒットストップ終了処理
	void EndHitStop();

	// アニメーションインスタンスに武器情報をセット
	void SetWeaponType_AnimInstance();

protected:

	//-----------------コンポーネント変数--------------------------------------

	// アクションのアシスト用コンポーネント
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Assist")
		class UActionAssistComponent* ActionAssistComp;

	//--------------------------------------------------------------------------

	// 武器の指定
	UPROPERTY(EditAnywhere, Category = "Weapon")
		TSubclassOf<AWeaponBase> WeaponClass;

	// 武器のソケットの名前
	UPROPERTY(EditAnywhere, Category = "Weapon")
		TArray<FName> SocketNames;

private:

	// ゲーム開始時に武器を持たせるかどうか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		bool bIsSpawnWeapon_BeginPlay;

	// キャラクターの基本ステータス
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
		FCharacterStatus Status;

	// アニメーションインスタンス格納
	class UCharacterAnimInstance* AnimInstance;

	// 武器クラスのポインタ
	AWeaponBase* Weapon;

	// 現在所持している武器を判別するEnum
	EWeaponType WeaponType;

	// タイマーハンドル
	FTimerHandle TimerHandle_HitStop;
};