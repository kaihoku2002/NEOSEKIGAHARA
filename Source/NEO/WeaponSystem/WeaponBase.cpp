// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "NEO/CharacterSystem/ActionAssistComponent.h"


// Sets default values
AWeaponBase::AWeaponBase()
	: IsPickUp(true)
	, IsFalling(false)
	, Frames(0.f)
	, Damage(0)
	, JumpHeight(150.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	// タグ設定
	Tags.Add("Weapon");

	// アタックアシストコンポーネント作成
	AuraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AuraEffect"));

	// アタックアシストコンポーネント作成
	ActionAssistComp = CreateDefaultSubobject<UActionAssistComponent>(TEXT("AttackAssist"));
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFalling)
	{
		BlowsAway();
	}
}

/*
 * 関数名　　　　：SetupWeaponMesh()
 * 処理内容　　　：プレイヤーのメッシュをセットアップ
 * 引数１　　　　：UStaticMeshComponent*& MeshComp・・・メッシュコンポーネント
 * 引数２　　　　：TCHAR* WeaponAssetPath ・・・・・・・武器のアセットのパス
 * 引数３　　　　：FName PublicName ・・・・・・・・・・エディタでの公開名
 * 戻り値　　　　：なし
 */
void AWeaponBase::SetupWeaponMesh(UStaticMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName /*= "WeaponMesh"*/)
{
	// 武器のコンポーネントを作成
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(PublicName);

	if (WeaponAssetPath)
	{
		// 武器のアセット設定
		ConstructorHelpers::FObjectFinder< UStaticMesh > weaponMesh(WeaponAssetPath);

		if (weaponMesh.Succeeded())
		{
			MeshComp->SetStaticMesh(weaponMesh.Object);

			// ルートに設定
			RootComponent = MeshComp;
		}

		// エフェクトを追従
		if (AuraEffect)
		{
			AuraEffect->SetupAttachment(RootComponent);
		}
	}
}

/*
 * 関数名　　　　：SetupWeaponMesh()
 * 処理内容　　　：プレイヤーのメッシュをセットアップ(引数がSkeletalMeshの場合)
 * 引数１　　　　：USkeletalMeshComponent*& MeshComp・・・メッシュコンポーネント
 * 引数２　　　　：TCHAR* WeaponAssetPath ・・・・・・・・武器のアセットのパス
 * 引数３　　　　：FName PublicName ・・・・・・・・・・・エディタでの公開名
 * 戻り値　　　　：なし
 */
void AWeaponBase::SetupWeaponMesh(USkeletalMeshComponent*& MeshComp, TCHAR* WeaponAssetPath, FName PublicName /*= "WeaponMesh"*/)
{
	// 武器のコンポーネントを作成
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(PublicName);

	if (WeaponAssetPath)
	{
		// 武器のアセット設定
		ConstructorHelpers::FObjectFinder< USkeletalMesh > weaponMesh(WeaponAssetPath);

		if (weaponMesh.Succeeded())
		{
			MeshComp->SetSkeletalMeshAsset(weaponMesh.Object);

			// ルートに設定
			RootComponent = MeshComp;
		}

		// エフェクトを追従
		if (AuraEffect)
		{
			AuraEffect->SetupAttachment(RootComponent);
		}
	}
}


/*
 * 関数名　　　　：AttachToCharacter()
 * 処理内容　　　：キャラクターにくっつける
 * 引数１　　　　：ACharacter* _owner ・・・・親になるキャラクター
 * 引数２　　　　：FName _socketName・・・・・ソケットの名前
 * 戻り値　　　　：なし
 */
void AWeaponBase::AttachToCharacter(ACharacter* _owner, FName _socketName)
{
	// 飛んでいるときは取れない
	if (!GetIsPickUp()) { return; }

	// 取得出来ない状態に
	IsPickUp = false;

	// オーナーの情報保存
	pOwner = _owner;

	if (AuraEffect)
	{
		// エフェクトを見えなくする
		AuraEffect->SetVisibility(false);
	}

	// キャラクターにアタッチ
	AttachToComponent(_owner->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false), _socketName);
}


/*
 * 関数名　　　　：DetachFromCharacter()
 * 処理内容　　　：キャラクターにくっつける
 * 戻り値　　　　：なし
 */
void AWeaponBase::DetachFromCharacter()
{
	// キャラクターから外す
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld,false));

	// 飛んでいるときは拾えない状態へ
	IsPickUp = true;

	// 吹き飛ばすフラグを建てる
	IsFalling = true;

	// オーナーを外す
	pOwner = nullptr;

	// 飛ぶ前の場所設定
	FlyBeforePos = GetActorLocation();
}


/*
 * 関数名　　　　：GetHitResults()
 * 処理内容　　　：攻撃が当たったかどうか判定(開始と終了を取らない)
 * 引数１　　　　：TArray<FHitResult>& _outHitResults・・・当たったオブジェクト
 * 戻り値　　　　：当たったかどうか
 */
bool AWeaponBase::GetHitResults(TArray<FHitResult>& _outHitResults)
{
	// 当たり判定を取る範囲
	FVector Start = WeaponCollision->GetComponentLocation();
	FVector End = Start;
	
	return GetHitResults(Start, End, _outHitResults);
}


/*
 * 関数名　　　　：GetHitResults()
 * 処理内容　　　：攻撃が当たったかどうか判定
 * 引数１　　　　：FVector _start ・・・・・・・・ ・・・・当たり判定をとる範囲(開始地点)
 * 引数２		 ：FVector _end・・・・・・・・・・・・・・当たり判定をとる範囲(終了地点)
 * 引数３		 ：TArray<FHitResult>& _outHitResults・・・当たったオブジェクト
 * 戻り値　　　　：当たったかどうか
 */
bool AWeaponBase::GetHitResults(FVector _start, FVector _end, TArray<FHitResult>& _outHitResults)
{
	// 武器本体とオーナーには当たらないようにする
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(pOwner);

	// 当たった部分を取得
	FQuat Rot = WeaponCollision->GetComponentQuat();
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(WeaponCollision->GetScaledCapsuleRadius(), WeaponCollision->GetScaledCapsuleHalfHeight());

	// あたっているか確認
	bool IsHit = GetWorld()->SweepMultiByChannel(_outHitResults, _start, _end, Rot, ECollisionChannel::ECC_GameTraceChannel1, CollisionShape, CollisionParams);

	return IsHit;
}


/*
 * 関数名　　　　：BlowsAway()
 * 処理内容　　　：武器が落ちて地面に刺さる
 * 戻り値　　　　：なし
 */
void AWeaponBase::BlowsAway()
{
	// 現在位置
	FVector NowPos = GetActorLocation();

	// Sinで高さ更新
	float SinValue = JumpHeight * FMath::Sin(RadPerFrame * Frames);

	// ジャンプ前の高さから位置更新
	const FVector nextPos(FVector(NowPos.X, NowPos.Y, SinValue + FlyBeforePos.Z));
	SetActorLocation(nextPos);

	// フレーム更新
	Frames += 1.f;

	// 現在の回転取得
	FRotator NowRotation = GetActorRotation();

	// 常に回転
	SetActorRotation(FRotator(NowRotation.Pitch, NowRotation.Yaw, NowRotation.Roll + 10.0));

	// 元の位置より低くなったら終了
	if (NowPos.Z < FlyBeforePos.Z)
	{
		// 持たれていない状態にする
		IsPickUp = true;

		// 飛んでいる状態のフラグを下ろす
		IsFalling = false;

		// フレームリセット
		Frames = 0.f;

		// 落下音再生
		if (DropWeaponSoundObj)
		{
			// 斬撃SE再生
			ActionAssistComp->PlaySound(DropWeaponSoundObj);
		}

		// 地面に刺さるように位置と角度を補正
		SetActorLocation(FlyBeforePos + DropLocation);
		SetActorRotation(DropAngle);

		if (AuraEffect)
		{
			// エフェクトを見えるようにする
			AuraEffect->SetVisibility(true);
		}
	}
}


