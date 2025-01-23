// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "NEO/WeaponSystem/Gun.h"
#include "NEO/AnimationSystem/CharacterAnimInstance.h"
#include "ActionAssistComponent.h"

// Sets default values
ACharacterBase::ACharacterBase()
	: bIsSpawnWeapon_BeginPlay(true)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// アタックアシストコンポーネント作成
	ActionAssistComp = CreateDefaultSubobject<UActionAssistComponent>(TEXT("AttackAssist"));
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// アニメーションインスタンス取得
	AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	// ゲーム開始時に武器をスポーン
	SpawnWeapon_BeginPlay();
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


/*
 * 関数名　　　　：HitStop()
 * 処理内容　　　：ヒットストップ
 * 引数１　　　　：float _speedDuringHitStop・・・ヒットストップ中のキャラクターの速度
 * 引数２　　　　：float _stopTime・・・・・・・・ヒットストップを掛ける時間
 * 戻り値　　　　：なし
 */
void ACharacterBase::HitStop(float _speedDuringHitStop, float _stopTime)
{
	// キャラクターのメッシュ取得
	USkeletalMeshComponent* CharacterMesh = GetMesh();

	// キャラクターのメッシュが取得できなかったらリターン
	if (!CharacterMesh) { return; }

	// HitStopを開始
	CharacterMesh->GlobalAnimRateScale = _speedDuringHitStop;

	// HitStopを停止
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(TimerHandle_HitStop, this, &ACharacterBase::EndHitStop, _stopTime, false);
}


// 前方後方になにかあるかチェック
bool ACharacterBase::WallCheck(float _lineLength)
{
	// レイキャストを実行する際のパラメータを設定する
	// レイキャストの開始位置はキャラクターの現在位置
	float Rotation_Z = GetActorRotation().Yaw;
	float LineLength = (Rotation_Z > 0) ? (_lineLength) : (-_lineLength);

	// 始点
	FVector start = GetActorLocation();
	// 終点
	FVector end = FVector(start.X, start.Y + LineLength, start.Z);

	FCollisionQueryParams CollisionParams;
	// オーナー自身は除外
	CollisionParams.AddIgnoredActor(this);

	FHitResult HitResult;

	// レイトレースを行う
	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, start, end, ECC_WorldStatic, CollisionParams);


	return IsHit;
}

/*
 * 関数名　　　　：InitCharaStatus()
 * 処理内容　　　：ステータス初期化
 * 戻り値　　　　：なし
 */
void ACharacterBase::InitStatus(int32 _hp, float _moveSpeed, float _jumpHeight, float _comboDamageFactor)
{
	// ステータス初期化
	Status = Status.InitParam(_hp, _moveSpeed, _jumpHeight, _comboDamageFactor);
}


/*
 * 関数名　　　　：EndHitStop()
 * 処理内容　　　：ヒットストップ終了
 * 戻り値　　　　：なし
 */
void ACharacterBase::EndHitStop()
{
	//HitStopを停止
	GetMesh()->GlobalAnimRateScale = 1.f;
}


/*
 * 関数名　　　　：SpawnWeapon_BeginPlay()
 * 処理内容　　　：ゲーム開始時に武器を出現
 * 戻り値　　　　：なし
 */
void ACharacterBase::SpawnWeapon_BeginPlay()
{
	// ゲーム開始時にスポーンしない設定の時はスルー
	if (!bIsSpawnWeapon_BeginPlay) { return; }

	// 初期武器が設定されているか確認(BPから設定可能)
	if (WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 武器をSpawn
		Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);

		// プレイヤーにアタッチ
		if (Weapon)
		{
			// 武器の種類取得
			WeaponType = Weapon->GetWeaponType();

			// AnimationInstanceに武器情報を送る
			SetWeaponType_AnimInstance();

			// ソケットが設定されているか判定
			if (SocketNames.Num() > 0)
			{
				Weapon->AttachToCharacter(this, SocketNames[0]);
			}
		}
	}
}


/*
 * 関数名　　　　：Attack_Gun()
 * 処理内容　　　：当たったオブジェクトをActorに変換
 * 戻り値　　　　：なし
 */
void ACharacterBase::Attack_Gun()
{
	// 武器がなかったらスルー
	if (!Weapon) { return; }

	// 銃の射撃処理
	Cast<AGun>(Weapon)->Shoot(50);
}


/*
 * 関数名　　　　：AttachWeapon()
 * 処理内容　　　：武器を装備する
 * 引数１　　　　：AWeaponBase* _newWeapon・・・取得する武器
 * 引数２		：FName _socketName ・・・武器をくっつけるためのソケットの名前
 * 引数３　　　　：bool _bDestroy・・・・・武器を持っている場合離すと同時に削除するか
 * 戻り値　　　　：装備が成功したか
 */
bool ACharacterBase::AttachWeapon(AWeaponBase* _newWeapon, FName _socketName /*= "None"*/, bool _bDestroy /*= true*/)
{
	// 新しい武器が存在するかつソケットが設定されているか
	if (!_newWeapon || SocketNames.Num() <= 0) { return false; }

	// 武器が取得できる状態か確認
	if (_newWeapon->GetIsPickUp())
	{
		// 他の武器を持っているとき
		if (Weapon)
		{
			// 武器を離す
			DetachWeapon(_bDestroy);
		}

		// 武器を更新
		Weapon = _newWeapon;
		WeaponType = Weapon->GetWeaponType();

		// AnimationInstanceに武器情報を送る
		SetWeaponType_AnimInstance();

		// 新しくソケットが指定されたら更新する
		const FName NewSocketName = (_socketName != "None") ? (_socketName) : (SocketNames[0]);

		Weapon->AttachToCharacter(this, NewSocketName);

		return true;
	}

	return false;
}


/*
 * 関数名　　　　：DetachWeapon()
 * 処理内容　　　：当たったオブジェクトをActorに変換
 * 戻り値　　　　：なし
 */
void ACharacterBase::DetachWeapon(bool _bDestroy /*= true*/)
{
	if (!Weapon) { return; }

	// 削除フラグ
	if (!_bDestroy)
	{
		// 今持っている武器を手放す
		Weapon->DetachFromCharacter();
	}
	else
	{
		// 武器を削除
		Weapon->Destroy();
	}

	// 武器を持っていない状態へ
	Weapon = nullptr;
	WeaponType = EWeaponType::WeaponType_None;

	// AnimationInstanceに武器情報を送る
	SetWeaponType_AnimInstance();
}


/*
 * 関数名　　　　：ChangeWeapon()
 * 処理内容　　　：新しい武器を装備する
 * 引数１　　　　：TSubclassOf<AWeaponBase> _newWeaponClass・・・新しい武器のクラス
 * 引数２        ：FName _socketName ・・・・・・・・・・・・・・ソケットの名前
 * 引数３　　　　：bool _bDestroy・・・・・・・・・・・・・・・・持ち替える前の武器を削除するかどうか
 * 戻り値　　　　：装備が成功したか
 */
bool ACharacterBase::ChangeWeapon(TSubclassOf<AWeaponBase> _newWeaponClass, FName _socketName /*= "None"*/, bool _bDestroy /*= true*/)
{
	// 新しい武器が設定されているか確認
	if (_newWeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 武器を持っている場合手離す
		if (Weapon)
		{
			// 今持っている武器を手放す
			Weapon->DetachFromCharacter();
		}

		// 新しい武器をSpawn
		Weapon = GetWorld()->SpawnActor<AWeaponBase>(_newWeaponClass, SpawnParams);

		// プレイヤーにアタッチ
		if (Weapon)
		{
			// 武器の種類取得
			WeaponType = Weapon->GetWeaponType();

			// くっつける
			Weapon->AttachToCharacter(this, _socketName);

			return true;
		}
	}

	return false;
}


/*
 * 関数名　　　　：HitObjToActor()
 * 処理内容　　　：当たったオブジェクトをActorに変換
 * 引数１　　　　：TArray<FHitResult> _hitResults・・・ヒットしたオブジェクト
 * 戻り値　　　　：変換したActorとその座標(タグを持っていないActorは省く)のタプル
 */
TTuple<TArray<AActor*>, TArray<FVector>> ACharacterBase::HitObjToActor(TArray<FHitResult> _hitResults)
{
	// Actorとその座標格納用
	TArray<AActor*> HitActors;
	TArray<FVector> HitLocations;

	// 当たったオブジェクトの数だけ繰り返し
	for (const FHitResult HitResult : _hitResults)
	{
		// Actor取得
		AActor* HitActor = HitResult.GetActor();

		if (HitActor->Tags.Num() > 0)
		{
			// タグを持っているActorとその座標のみを追加
			HitActors.Add(HitActor);
			HitLocations.Add(HitResult.Location);
		}
	}

	// タプルを返す
	return MakeTuple(HitActors, HitLocations);
}


/*
 * 関数名　　　　：SetWeaponType_AnimInstance()
 * 処理内容　　　：AnimationInstanceに武器情報をセット
 * 戻り値　　　　：なし
 */
void ACharacterBase::SetWeaponType_AnimInstance()
{
	// インスタンスが取得できていないときスルー
	if (!AnimInstance) { return; }

	// AnimationInstanceに武器情報を送る
	AnimInstance->SetWeaponType(WeaponType);
}



/*
 * 関数名　　　　：SetCollision()
 * 処理内容　　　：攻撃の当たり判定を発生させる
 * 戻り値　　　　：なし
 */
void ACharacterBase::SetCollision()
{
	if (!Weapon) { return; }

	// 当たったオブジェクト格納用
	TArray<FHitResult> OutHitResults;

	// 武器の種類によって処理を変える
	switch (WeaponType)
	{
	case EWeaponType::WeaponType_Sword:						// 刀の攻撃

		// 武器が何かに当たっていたら
		if (Weapon->GetHitResults(OutHitResults))
		{
			Attack_Sword(HitObjToActor(OutHitResults));
		}

		break;
	case EWeaponType::WeaponType_Lance:						// 槍の攻撃
														
		// 武器が何かに当たっていたら					
		if (Weapon->GetHitResults(OutHitResults))		
		{												
			Attack_Lance(HitObjToActor(OutHitResults));
		}

		break;
	case EWeaponType::WeaponType_Gun:						// 銃の攻撃

		Attack_Gun();										

		break;
	case EWeaponType::WeaponType_None:
		break;
	}

}


/*
 * 関数名　　　　：GetAnimationAsset()
 * 処理内容　　　：アニメーションアセットを返す
 * 引数１　　　　：TCHAR* _animAssetPath ・・・アニメーションアセットのパス
 * 戻り値　　　　：見つかったアニメーション
 */
UAnimMontage* ACharacterBase::GetAnimationAsset(TCHAR* _animAssetPath)
{
	// アセットを探してセット
	ConstructorHelpers::FObjectFinder<UAnimMontage> SearchAnimMontage(_animAssetPath);

	// そのアセットが存在したら格納
	if (SearchAnimMontage.Succeeded())
	{
		UAnimMontage* FoundAnimMontage = SearchAnimMontage.Object;

		return FoundAnimMontage;
	}

	return nullptr;
}