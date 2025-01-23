// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"


class UBoxComponent;		// -------------------------------
class UCapsuleComponnet;	// コリジョン作成テンプレート用
class USphereComponent;		// -------------------------------
class ACharacter;
class UNiagaraSystem;

//-----------------武器の種類-------------------------------------
UENUM(BlueprintType)
enum class EWeaponType :uint8
{
	WeaponType_Sword   UMETA(DisplayName = "Sword"),
	WeaponType_Lance   UMETA(DisplayName = "Lance"),
	WeaponType_Gun     UMETA(DisplayName = "Gun"),
	WeaponType_None	   UMETA(DisplayName = "None")
};
//----------------------------------------------------------------


UCLASS()
class NEO_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// コンストラクタ
	AWeaponBase();

public:

	// プレイヤーの手に付ける
	void AttachToCharacter(ACharacter* _owner, FName _socketName);

	// プレイヤーの手から外れる
	void DetachFromCharacter();

	// 飛んでいる状態か
	bool GetIsFalling()const { return IsFalling; }

	// 武器の種類判別用
	EWeaponType GetWeaponType()const { return WeaponType; }

	// 取得できる状態か
	bool GetIsPickUp()const { return IsPickUp && !IsFalling; }

	// 攻撃力取得
	int32 GetDamage()const { return Damage; }

	// 攻撃が当たったかどうか
	bool GetHitResults(TArray<FHitResult>& _outHitResults);
	bool GetHitResults(FVector _start, FVector _end, TArray<FHitResult>& _outHitResults);

	// コリジョン取得用
	class UCapsuleComponent* GetCollisionComponent()const { return WeaponCollision; }

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 引数によってスタティックメッシュかスケルタルメッシュのセットアップ
	void SetupWeaponMesh(UStaticMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName = "WeaponStaticMesh");
	void SetupWeaponMesh(USkeletalMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName = "WeaponSkeletalMesh");

	// ダメージ量設定
	void SetDamageAmount(int32 _damage) { Damage = _damage; }


private:

	// オーナーがいなくなったとき武器が落ちる
	void BlowsAway();


protected:

	// 武器のメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponMesh")
		class UStaticMeshComponent* WeaponStaticMesh;

	// 武器のコリジョン
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponCollision")
		class UCapsuleComponent* WeaponCollision;

	// アクション補助の役割をするコンポーネント
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActionAssist")
		class UActionAssistComponent* ActionAssistComp;

	// 武器が落ちているときのエフェクト
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
		class UNiagaraComponent* AuraEffect;

	// 武器を落とした時の音
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
		class USoundBase* DropWeaponSoundObj;

	// 武器を判別するEnum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
		FVector DropLocation;

	// 武器を判別するEnum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
		FRotator DropAngle;


	// 武器を判別するEnum
	EWeaponType WeaponType;

	// オーナーの情報
	class ACharacter* pOwner;

private:


	// 取得できる状態か
	bool IsPickUp;

	// 飛んでいるかどうか
	bool IsFalling;

	// フレームカウント用
	float Frames;

	// 攻撃力
	UPROPERTY(EditAnywhere,Category = "Status",meta = (AllowPrivateAcces = "true"))
		int32 Damage;

	// ジャンプの高さ
	UPROPERTY(EditAnywhere, Category = "Status", meta = (AllowPrivateAcces = "true"))
		float JumpHeight;

	// ジャンプの計算
	const float RadPerFrame = 3.14f / 30.f;

	// 飛ぶ前の位置
	FVector FlyBeforePos;

	// ハンドル
	FTimerHandle TimerHandle;


protected:

	// ---------コリジョンコンポーネント作成テンプレート
	/*
	 * 関数名　　　　：SetupCollisionComponent()
	 * 処理内容　　　：武器のコリジョンセットアップ
	 * 引数１　　　　：T* CollisionComp・・・・・・・・・コリジョンコンポーネント(「T」は UBoxComponent,USphereComponent,UCapsuleComponent のいずれか)
	 * 引数２　　　　：FName PublicName・・・・・・・・・エディタでの公開名
	 * 戻り値　　　　：なし
	 */
		template<class T>
		void SetupCollisionComponent(T*& CollisionComp, FName PublicName = "CollisionComp")
		{
			static_assert(std::is_same<T, UBoxComponent>::value || std::is_same<T, USphereComponent>::value || std::is_same<T, UCapsuleComponent>::value,
				"「T」は UBoxComponent,USphereComponent,UCapsuleComponent のいずれか ");
	
			// 対応するオブジェクト生成
			CollisionComp = CreateDefaultSubobject<T>(PublicName);
	
			if (CollisionComp)
			{
				// 武器のメッシュに追従
				CollisionComp->SetupAttachment(WeaponStaticMesh, "hand_rSocket");
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------
};
