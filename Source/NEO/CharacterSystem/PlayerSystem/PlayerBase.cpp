// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DebugComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NEOPlayerController.h"
#include "NEO/GameSystem/NEOGameMode.h"
#include "NEO/BackGroundSystem/ObjectBase.h"
#include "NEO/CharacterSystem/CharacterBase.h"
#include "NEO/CharacterSystem/ActionAssistComponent.h"
#include "NEO/WeaponSystem/Gun.h"

// Sets default values
APlayerBase::APlayerBase()
	: State(EPlayerState::Idle)
	, IsInvincibility(false)
	, frames(0.f)
	, CanCombo(true)
	, ComboIndex(0)
{
	// Tick()をマイフレーム呼ぶかどうかの判定
	PrimaryActorTick.bCanEverTick = true;

	// プレイヤーの設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// プレイヤーに回転の制限
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// タグ設定
	Tags.Add("Player");

	// Playerのセットアップ
	InitPlayerData();
}


// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	// ステータス設定
	SetupPlayerStatus();

	// ゲームモード取得
	pGameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// コントローラー取得
	PlayerController = Cast<ANEOPlayerController>(Controller);

	// 入力の設定
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainActionMapping.DefaultMappingContext, 0);
			Subsystem->AddMappingContext(DebugComponent->GetDebugKeyMapping().DebugKeyMappingContext, 1);
		}
	}
}


// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ジャンプ中のみ処理
	if (State == EPlayerState::Jump)
	{
		Jump();
	}
}


// 入力のバインド
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 移動
		EnhancedInputComponent->BindAction(MainActionMapping.MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);
		EnhancedInputComponent->BindAction(MainActionMapping.MoveAction, ETriggerEvent::Completed, this, &APlayerBase::Stop);


		// ジャンプ
		EnhancedInputComponent->BindAction(MainActionMapping.JumpAction, ETriggerEvent::Started, this, &APlayerBase::JumpStart);

		// 攻撃アクション
		EnhancedInputComponent->BindAction(MainActionMapping.ComboAction1, ETriggerEvent::Started, this, &APlayerBase::Attack1);
		EnhancedInputComponent->BindAction(MainActionMapping.ComboAction2, ETriggerEvent::Started, this, &APlayerBase::Attack2);


		if (!DebugComponent) { return; }
		// デバッグキーの設定
		DebugComponent->SetupDebugInputComponent(PlayerInputComponent);
	}

}


/*
 * 関数名　　　　：SetupDebugEventBindings()
 * 処理内容　　　：デバッグキーに関数をバインド
 * 戻り値　　　　：なし
 */
void APlayerBase::SetupDebugEventBindings()
{
	if (!DebugComponent) { return; }

	// ESC		：ゲーム終了
	// F１		：リスタートゲーム
	// F２		：
	// F３		：ポーズ
	// F４		：
	// F５		：
	// F６		：無敵化
	// F７		：死亡
	// F８		：
	// F９		：
	// F１０	：
	// F１１	：
	// F１２	：

	DebugComponent->DebugInputReceived_ESC.AddDynamic(this, &APlayerBase::QuitGame);
	DebugComponent->DebugInputReceived_F1.AddDynamic(this, &APlayerBase::RestartGame);
	DebugComponent->DebugInputReceived_F3.AddDynamic(this, &APlayerBase::SetGamePause);
	DebugComponent->DebugInputReceived_F6.AddDynamic(this, &APlayerBase::SetAbsolutelyInvincible);
	DebugComponent->DebugInputReceived_F7.AddDynamic(this, &APlayerBase::SetDeath);
}


/*
 * 関数名　　　　：SetupPlayerData()
 * 処理内容　　　：プレイヤーのデータ初期化
 * 戻り値　　　　：なし
 */
void APlayerBase::InitPlayerData()
{
	// デバッグコンポーネント作成作成
	DebugComponent = CreateDefaultSubobject<UDebugComponent>(TEXT("Debug"));

	// デバッグ用関数バインド
	SetupDebugEventBindings();

	// コリジョンイベントを設定
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnOverlap);

	// ステータス初期化
	InitStatus(2, 500.f, 150.f, 1.f);

	InvincibilityTime_Short = 0.3f;
	InvincibilityTime_Long = 0.5f;

	// 武器種によってソケットを変更
	SocketNames.Add("hand_rSocket_Sword");
	SocketNames.Add("hand_rSocket_Lance");
	SocketNames.Add("hand_rSocket_Gun");

	// コンボの名前格納
	ComboStartSectionNames = { "First", "Second", "Third","Fourth" };
}


/*
 * 関数名　　　　：SetupPlayerStatus()
 * 処理内容　　　：プレイヤーのステータス設定
 * 戻り値　　　　：なし
 */
void APlayerBase::SetupPlayerStatus()
{
	// 移動速度設定
	CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->MaxWalkSpeed = GetStatus().MoveSpeed;

	// 武器を落とすまでの被ダメージ回数設定
	DefaultDamageLimit = GetStatus().HP;
}


/*
 * 関数名　　　　：Move()
 * 処理内容　　　：プレイヤーの入力受付(移動処理)
 * 引数１　　　　：FInputActionValue& Value・・・入力量
 * 戻り値　　　　：なし
 */
void APlayerBase::Move(const FInputActionValue& _value)
{
	// 停止・移動・ジャンプ中のみ可
	if (!(State == EPlayerState::Idle || State == EPlayerState::Move || State == EPlayerState::Jump)) { return; }

	// 2軸で入力取得
	FVector2D MovementVector = _value.Get<FVector2D>();

	if (PlayerController)
	{
		// カメラの角度取得
		const FRotator Rotation = PlayerController->GetNowCameraRotation();

		// 回転設定
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		// カメラの向きに沿った移動方向取得(X,Y)
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 移動
		AddMovementInput(RightDirection, MovementVector.X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		// 移動方向に回転
		RotateCharacter(MovementVector.Y);
	}

	// 止まっている時からのみ変更
	if (State == EPlayerState::Idle)
	{
		// 移動中へ
		State = EPlayerState::Move;
	}
}


/*
 * 関数名　　　　：Stop()
 * 処理内容　　　：プレイヤーの入力受付(停止処理)
 * 戻り値　　　　：なし
 */
void APlayerBase::Stop()
{
	// 移動中以外はスルー
	if (State != EPlayerState::Move) { return; }

	// 停止へ
	State = EPlayerState::Idle;
}


/*
 * 関数名　　　　：JumpStart()
 * 処理内容　　　：プレイヤーの入力受付(ジャンプ開始)
 * 戻り値　　　　：なし
 */
void APlayerBase::JumpStart()
{
	// 停止中と移動中のみジャンプ可
	if (State == EPlayerState::Idle || State == EPlayerState::Move)
	{
		// ジャンプ時間
		frames = 0.f;

		// ジャンプ前の高さ取得
		JumpBeforePos_Z = GetActorLocation().Z;

		// ステートをジャンプ処理へ
		State = EPlayerState::Jump;
	}
}


/*
 * 関数名　　　　：Jump()
 * 処理内容　　　：プレイヤーのジャンプ中処理
 * 戻り値　　　　：なし
 */
void APlayerBase::Jump()
{
	// 現在位置
	FVector NowPos = GetActorLocation();

	// Sinで高さ更新
	float SinValue = GetStatus().JumpHeight * FMath::Sin(radPerFrame * frames);

	// ジャンプ前の高さから位置更新
	const FVector nextPos(FVector(NowPos.X, NowPos.Y, SinValue + JumpBeforePos_Z));

	SetActorLocation(nextPos);

	// 着地処理 下降開始から判定開始
	if (frames >= 10.f && (IsPlayerGrounded() || nextPos.Z <= JumpBeforePos_Z))
	{
		State = EPlayerState::Idle;
	}

	// フレーム+1
	frames += 1.f;
}


/*
 * 関数名　　　　：IsPlayerGrounded()
 * 処理内容　　　：プレイヤーが地面についているか判定
 * 戻り値　　　　：地面についていたらtrue
 */
bool APlayerBase::IsPlayerGrounded() const
{
	if (!CharacterMovementComp) { return false; }

	// 接地しているか
	bool IsGrounded = CharacterMovementComp->IsFalling() == false;

	return IsGrounded;
}


// 接触開始時に行う処理
void APlayerBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 被ダメージ中は武器を拾えない状態に
	if (State == EPlayerState::Damage || State == EPlayerState::Death) { return; }

	// オーバーラップした際に実行したいイベント
	if (OtherActor && (OtherActor != this))
	{
		// 当たったのがプレイヤーの時装備させる
		if (OtherActor->ActorHasTag("Weapon"))
		{
			// 新しい武器の情報取得
			AWeaponBase* NewWeapon = Cast<AWeaponBase>(OtherActor);

			// 武器を拾う
			if (AttachWeapon(NewWeapon, SocketNames[int32(NewWeapon->GetWeaponType())], false))
			{
				// 武器を落とすまでの被ダメージ回数リセット
				DefaultDamageLimit = GetStatus().HP;
			}
		}
	}
}


/*
 * 関数名　　　　：ComboAttack()
 * 処理内容　　　：プレイヤーの攻撃処理
 * 引数１　　　　：int _attackNum・・・攻撃アニメーションの種類判別用
 * 戻り値　　　　：なし
 */
void APlayerBase::ComboAttack(int _attackNum /*= 0*/)
{
	switch (GetWeaponType())
	{
	case EWeaponType::WeaponType_Sword:
		PlayComboAnimtion_Sword(_attackNum);
		break;
	case EWeaponType::WeaponType_Lance:
		PlayComboAnimtion_Lance(_attackNum);
		break;
	case EWeaponType::WeaponType_Gun:
		PlayComboAnimtion_Gun(_attackNum);
		break;
	case EWeaponType::WeaponType_None:

		break;
	}
}


/*
 * 関数名　　　　：Attack1()
 * 処理内容　　　：プレイヤーの入力受付(攻撃１つ目)
 * 戻り値　　　　：なし
 */
void APlayerBase::Attack1()
{
	if (!(State == EPlayerState::Idle || State == EPlayerState::Move || State == EPlayerState::Jump || State == EPlayerState::Attack)) { return; }

	// コンボ攻撃
	ComboAttack(0);
}

/*
 * 関数名　　　　：Attack2()
 * 処理内容　　　：プレイヤーの入力受付(攻撃２つ目)
 * 戻り値　　　　：なし
 */
void APlayerBase::Attack2()
{
	if (!(State == EPlayerState::Idle || State == EPlayerState::Move || State == EPlayerState::Jump || State == EPlayerState::Attack)) { return; }

	// コンボ攻撃
	ComboAttack(1);
}


/*
 * 関数名　　　　：Attack_Sword()
 * 処理内容　　　：刀が当たった時の処理
 * 引数１　　　　：TArray<AActor*> _otherActors・・・刀に当たったActorと当たった位置の情報のタプル
 * 戻り値　　　　：なし
 */
void APlayerBase::Attack_Sword(TTuple<TArray<AActor*>, TArray<FVector>> HitActorAndLocation)
{
	// 武器が取得できないとき処理しない
	if (!GetWeapon()) { return; }

	// 当たったActorを取得
	TArray<AActor*> HitActors = HitActorAndLocation.Get<0>();
	TArray<FVector> HitLocations = HitActorAndLocation.Get<1>();


	// ヒットしたActorの数だけ繰り返し
	for (int i = 0; i < HitActors.Num(); ++i)
	{
		// Actorとその座標取得
		AActor* HitActor = HitActors[i];
		const FVector HitLocation = HitLocations[i];

		// オブジェクトに当たっている場合
		if (HitActor && HitActor->ActorHasTag("Object"))
		{
			// 破壊可能オブジェクトの情報取得
			AObjectBase* BreakObj = Cast<AObjectBase>(HitActor);

			// 破壊!!
			if (BreakObj)
			{
				BreakObj->ReceiveDamage(GetDamageAmount());
			}
			break;
		}
		// 敵に当たっている場合
		else if (HitActor && HitActor->ActorHasTag("Enemy"))
		{
			// ヒットした敵取得
			ACharacterBase* HitEnemy = Cast<ACharacterBase>(HitActor);

			// 敵への攻撃
			if (HitEnemy)
			{
				// エネミーに攻撃
				HitEnemy->TakedDamage(GetDamageAmount());
			}

			// ヒットストップ
			HitStop(HitStopSpeed, HitStopTime);

			// ヒットエフェクト表示
			if (HitEffect)
			{
				ActionAssistComp->SpawnEffect(HitEffect, HitLocations[i]);
			}

			// コンボの最後にカメラを揺らす
			if (GetComboIndex() == 3)
			{
				ActionAssistComp->CameraShake(ShakePattern);
			}

			// 刀用のヒット音再生
			ActionAssistComp->PlaySound(PlayerSounds.SwordAttack);
		}
	}
}


/*
 * 関数名　　　　：Attack_Lance()
 * 処理内容　　　：槍が当たった時の処理
 * 引数１　　　　：TTuple<TArray<AActor*>, TArray<FVector>> HitActorAndLocation・・・槍に当たったActorと当たった位置の情報のタプル
 * 戻り値　　　　：なし
 */
void APlayerBase::Attack_Lance(TTuple<TArray<AActor*>, TArray<FVector>> HitActorAndLocation)
{
	// 武器が取得できないとき処理しない
	if (!GetWeapon()) { return; }

	// 当たったActorを取得
	TArray<AActor*> HitActors = HitActorAndLocation.Get<0>();
	TArray<FVector> HitLocations = HitActorAndLocation.Get<1>();


	// ヒットしたActorの数だけ繰り返し
	for (int i = 0; i < HitActors.Num(); ++i)
	{
		// Actorとその座標取得
		AActor* HitActor = HitActors[i];
		const FVector HitLocation = HitLocations[i];

		// オブジェクトに当たっている場合
		if (HitActor && HitActor->ActorHasTag("Object"))
		{
			// 破壊可能オブジェクトの情報取得
			AObjectBase* BreakObj = Cast<AObjectBase>(HitActor);

			// 破壊!!
			if (BreakObj)
			{
				BreakObj->ReceiveDamage(GetDamageAmount());
			}
			break;
		}
		// 敵に当たっている場合
		else if (HitActor && HitActor->ActorHasTag("Enemy"))
		{
			// ヒットした敵取得
			ACharacterBase* HitEnemy = Cast<ACharacterBase>(HitActor);

			// 敵への攻撃
			if (HitEnemy)
			{
				// エネミーに攻撃
				HitEnemy->TakedDamage(GetDamageAmount());
			}

			// ヒットストップ
			HitStop(HitStopSpeed, HitStopTime);

			// ヒットエフェクト表示
			if (HitEffect)
			{
				ActionAssistComp->SpawnEffect(HitEffect, HitLocations[i]);
			}

			// コンボの最後にカメラを揺らす
			if (GetComboIndex() == 3)
			{
				ActionAssistComp->CameraShake(ShakePattern);
			}

			// 槍用のヒット音再生
			ActionAssistComp->PlaySound(PlayerSounds.LanceAttack);
		}
	}
}


/*
 * 関数名　　　　：Attack_Gun()
 * 処理内容　　　：銃の攻撃処理
 * 戻り値　　　　：なし
 */
void APlayerBase::Attack_Gun()
{
	// 武器が取得できないとき処理しない
	if (!GetWeapon()) { return; }

	if (!IsKicking)
	{
		// 銃の射撃処理
		Cast<AGun>(GetWeapon())->Shoot(GetDamageAmount() * 5);

		// 射撃音再生
		ActionAssistComp->PlaySound(PlayerSounds.Shoot);
	}
	else
	{
		// 自分と武器には当たらないようにする
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		CollisionParams.AddIgnoredActor(GetWeapon());

		// 自分の当たり判定取得
		UCapsuleComponent* CapsuleCollision = GetCapsuleComponent();

		// 当たった部分を取得
		FQuat Rot = CapsuleCollision->GetComponentQuat();
		FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleCollision->GetScaledCapsuleRadius(), CapsuleCollision->GetScaledCapsuleHalfHeight());

		// 足元の位置を取得
		FVector Start = GetMesh()->GetSocketLocation("player_finished1_R_ankle");
		FVector End = Start;

		// 当たったオブジェクトを格納する
		TArray<FHitResult> OutHitResults;

		// あたっているか確認
		bool IsHit = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, Rot, ECollisionChannel::ECC_GameTraceChannel1, CollisionShape, CollisionParams);

		// 当たっていなかったらスルー
		if (!IsHit) { return; }


		for (FHitResult OutHitResult : OutHitResults)
		{
			// 当たったActor取得
			AActor* HitActor = OutHitResult.GetActor();

			// オブジェクトに当たっている場合
			if (HitActor && HitActor->ActorHasTag("Object"))
			{
				// 破壊可能オブジェクトの情報取得
				AObjectBase* BreakObj = Cast<AObjectBase>(HitActor);

				// 破壊!!
				if (BreakObj)
				{
					BreakObj->ReceiveDamage(GetDamageAmount());
				}
				break;
			}
			// 敵に当たっている場合
			else if (HitActor && HitActor->ActorHasTag("Enemy"))
			{
				// ヒットした敵取得
				ACharacterBase* HitEnemy = Cast<ACharacterBase>(HitActor);

				if (HitEnemy)
				{
					// 敵への攻撃
					HitEnemy->TakedDamage(GetDamageAmount());
				}

				// ヒットストップ
				HitStop(HitStopSpeed, HitStopTime);

				// ヒットエフェクト表示
				if (HitEffect)
				{
					ActionAssistComp->SpawnEffect(HitEffect, OutHitResult.Location);
				}

				// 蹴り用のヒット音再生
				ActionAssistComp->PlaySound(PlayerSounds.Kick);

				break;
			}
		}
	}
}


/*
 * 関数名　　　　：PlayComboAnimtion_Sword()
 * 処理内容　　　：刀の攻撃アニメーション
 * 戻り値　　　　：なし
 */
void APlayerBase::PlayComboAnimtion_Sword(int _attackNum)
{
	// コンボ不可の時スルー
	if (!CanCombo) { return; }

	if (State != EPlayerState::Jump)
	{
		if (State != EPlayerState::Attack)
		{
			// 攻撃中フラグオン
			State = EPlayerState::Attack;
		}
		else
		{
			// ラストアタックまでコンボ継続
			if (ComboStartSectionNames[ComboIndex] != ComboStartSectionNames.Last())
			{
				++ComboIndex;
			}
		}

		// 攻撃のアニメーション再生
		PlayAnimation(PlayerAnimation.ComboAttack[_attackNum], ComboStartSectionNames[ComboIndex]);
	}
	else
	{
		// 攻撃中フラグオン
		State = EPlayerState::Attack;

		// 攻撃のアニメーション再生
		PlayAnimation(PlayerAnimation.AirAttack);
		ComboIndex = 2;
	}
}


/*
 * 関数名　　　　：PlayComboAnimtion_Lance()
 * 処理内容　　　：槍のコンボ
 * 戻り値　　　　：なし
 */
void APlayerBase::PlayComboAnimtion_Lance(int _attackNum)
{
	// コンボ不可の時スルー
	if (!CanCombo) { return; }

	if (State != EPlayerState::Jump)
	{
		if (State != EPlayerState::Attack)
		{
			// 攻撃中フラグオン
			State = EPlayerState::Attack;
		}
		else
		{
			// ラストアタックまでコンボ継続
			if (ComboStartSectionNames[ComboIndex] != ComboStartSectionNames.Last())
			{
				++ComboIndex;
			}
		}

		// 攻撃のアニメーション再生
		PlayAnimation(PlayerAnimation.ComboAttack[_attackNum], ComboStartSectionNames[ComboIndex]);
	}
	else
	{
		// 攻撃中フラグオン
		State = EPlayerState::Attack;

		// 攻撃のアニメーション再生
		PlayAnimation(PlayerAnimation.AirAttack);
		ComboIndex = 2;
	}
}


/*
 * 関数名　　　　：PlayComboAnimtion_Gun()
 * 処理内容　　　：銃の攻撃アニメーション
 * 戻り値　　　　：なし
 */
void APlayerBase::PlayComboAnimtion_Gun(int _attackNum)
{
	if (_attackNum == 0)
	{
		IsKicking = true;
		PlayAnimation(PlayerAnimation.GunAttack2);
	}
	else
	{
		PlayAnimation(PlayerAnimation.GunAttack);
	}

	// 攻撃中フラグオン
	State = EPlayerState::Attack;
}


/*
 * 関数名　　　　：RotateCharacter()
 * 処理内容　　　：プレイヤーのステータス初期化
 * 引数１　　　　：float _nowInput_X・・・現在の移動入力値
 * 戻り値　　　　：なし
 */
void APlayerBase::RotateCharacter(float _nowInput_X)
{
	// 入力がない場合は何もしない
	if (_nowInput_X == 0) { return; }

	// 右を向いているか確認
	bool bLookRight = (_nowInput_X != 1.f) ? (true) : (false);

	// 回転
	ActionAssistComp->OwnerParallelToCamera(bLookRight);
}


/*
 * 関数名　　　　：SlowDownDeathAnimationRate()
 * 処理内容　　　：死亡時アニメーション引き伸ばし
 * 戻り値　　　　：なし
 */
void APlayerBase::SlowDownDeathAnimationRate()
{
	// 遅くする
	GetMesh()->GlobalAnimRateScale = DeadAnimRate;


	// プレイヤーを削除
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(TimerHandle, this, &APlayerBase::CallControllerFunc_DestroyPlayer, DeadToGameOverTime, false);
}



/*
 * 関数名　　　　：CallControllerFunc_DestroyPlayer()
 * 処理内容　　　：死亡時のゲームモード内の関数呼び出し
 * 　　　　　　　　残機が残ってたらリスポーン,なくなったらプレイヤー削除
 * 戻り値　　　　：なし
 */
void APlayerBase::CallControllerFunc_DestroyPlayer()
{
	// コントローラーからプレイヤーを削除
	if (PlayerController)
	{
		// 残機があるうちはリスポーン
		if (PlayerController->GetRemainingLives() > 0)
		{
			// プレイヤーリスポーン
			PlayerController->RespawnPlayer();
		}
		// それ以外はゲームオーバー
		else
		{
			// プレイヤー削除
			PlayerController->DestroyPlayer();
		}
	}
}


/*
 * 関数名　　　　：ResetAllAttackFlags()
 * 処理内容　　　：攻撃に関するフラグをすべてリセット
 * 戻り値　　　　：なし
 */void APlayerBase::ResetAllAttackFlags()
 {
	 // 攻撃中のフラグリセット
	 CanCombo = true;
	 IsKicking = false;
	 ComboIndex = 0;
	 State = EPlayerState::Idle;
 }


 /*
  * 関数名　　　　：ContinuationCombo()
  * 処理内容　　　：コンボの継続
  * 戻り値　　　　：なし
  */
 void APlayerBase::ContinuationCombo()
 {
	 CanCombo = true;
 }


 /*
  * 関数名　　　　：ResetCombo()
  * 処理内容　　　：コンボリセット
  * 戻り値　　　　：なし
  */
 void APlayerBase::ResetCombo()
 {
	 // 攻撃中のフラグをすべてリセット
	 ResetAllAttackFlags();
 }


 /*
  * 関数名　　　　：StopMontage()
  * 処理内容　　　：アニメーションを止める
  * 戻り値　　　　：なし
  */
 void APlayerBase::StopMontage()
 {
	 //アニメーションを止める
	 GetMesh()->GetAnimInstance()->StopSlotAnimation();
 }


 /*
* 関数名　　　　：SetInvincibilityOn()
* 処理内容　　　：被ダメージ時に無敵化
* 戻り値　　　　：なし
*/
 void APlayerBase::SetInvincibilityOn()
 {
	 // 無敵を解除してステートを被ダメージ状態に
	 IsInvincibility = true;
	 State = EPlayerState::Damage;
 }


 /*
 * 関数名　　　　：SetInvincibilityOff()
 * 処理内容　　　：無敵時間後に無敵解除
 * 戻り値　　　　：なし
 */
 void APlayerBase::SetInvincibilityOff()
 {
	 // 無敵を解除してステートを通常状態に
	 IsInvincibility = false;
	 State = EPlayerState::Idle;
 }

 /*
  * 関数名　　　　：TakedDamage()
  * 処理内容　　　：プレイヤーの被ダメージ処理
  * 引数１　　　　：float _damage・・・被ダメージ量
  * 引数２　　　　：bool _isLastAttack・・・コンボの最終かどうか
  * 戻り値　　　　：なし
  */
 void APlayerBase::TakedDamage(float _damage, bool _isLastAttack /*= false*/)
 {
	 if (State == EPlayerState::Death || IsInvincibility || AbsolutelyInvincible) { return; }

	 // 武器を持っていないときに攻撃を受けたら死亡
	 if (!GetWeapon() && State != EPlayerState::Death)
	 {
		 // 死亡状態へ
		 State = EPlayerState::Death;

		 // コリジョンをオフに
		 SetActorEnableCollision(true);

		 // ヒットエフェクト発生
		 ActionAssistComp->SpawnEffect(HitEffect, GetActorLocation());

		 // 死亡アニメーション再生
		 PlayAnimation(PlayerAnimation.Death);
	 }
	 // 武器を持っているとき
	 else
	 {
		 // 攻撃中のフラグをすべてリセット
		 ResetAllAttackFlags();

		 // 無敵にする
		 IsInvincibility = true;

		 // 敵のコンボが最終段だった時必ず武器を落とす
		 if (_isLastAttack)
		 {
			 DefaultDamageLimit = 0;
		 }

		 // 無敵解除時間格納用
		 float InvincibilityReleaseTime;

		 // 被ダメージアニメーション
		 if (DefaultDamageLimit <= 0)
		 {
			 // 長い無敵時間を適用
			 InvincibilityReleaseTime = InvincibilityTime_Long;

			 // 武器を落とす
			 DetachWeapon(false);

			 // ノックバックアニメーション再生
			 PlayAnimation(PlayerAnimation.KnockBack);
		 }
		 else
		 {
			 // 短い無敵時間を適用
			 InvincibilityReleaseTime = InvincibilityTime_Long;

			 // 攻撃を受ける
			 --DefaultDamageLimit;

			 // のけぞりアニメーション再生
			 PlayAnimation(PlayerAnimation.TakeDamage);
		 }

		 // 無敵設定
		 SetInvincibilityOn();

		 // 一定時間後に無敵解除
		 FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		 TimerManager.SetTimer(TimerHandle, this, &APlayerBase::SetInvincibility, InvincibilityReleaseTime, false);

		 // ヒットエフェクト発生
		 ActionAssistComp->SpawnEffect(HitEffect, GetActorLocation());
	 }
 }

 //-----------------------デバッグ用関数-----------------------------------------------------------
 /*
  * 関数名　　　　：QuitGame()
  * 処理内容　　　：ESC:ゲーム終了
  * 戻り値　　　　：なし
  */
 void APlayerBase::QuitGame()
 {
	 // ゲーム終了
	 UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
 }


 /*
  * 関数名　　　　：QuitGame()
  * 処理内容　　　：F1:ゲームリスタート
  * 戻り値　　　　：なし
  */
 void APlayerBase::RestartGame()
 {
	 if (!pGameMode) { return; }

	 // ゲームモードからリスタート処理を呼ぶ
	 pGameMode->RestartGame();
 }


 /*
  * 関数名　　　　：SetGamePause()
  * 処理内容　　　：F3:ゲームポーズ
  * 戻り値　　　　：なし
  */
 void APlayerBase::SetGamePause()
 {
	 if (!pGameMode) { return; }

	 // ポーズの切り替え
	 bool IsPaused = !(pGameMode->IsPaused());

	 // ポーズ設定
	 pGameMode->SetGamePause(IsPaused);
 }


 /*
  * 関数名　　　　：SetAbsolutelyInvincible()
  * 処理内容　　　：F6:無敵
  * 戻り値　　　　：なし
  */
 void APlayerBase::SetAbsolutelyInvincible()
 {
	 // 無敵のオンオフ
	 AbsolutelyInvincible = !AbsolutelyInvincible;
 }


 /*
  * 関数名　　　　：SetDeath()
  * 処理内容　　　：デバッグ用死亡
  * 戻り値　　　　：なし
  */
 void APlayerBase::SetDeath()
 {
	 // 死亡アニメーション再生
	 PlayAnimation(PlayerAnimation.Death);
 }

 //------------------------------------------------------------------------------------------------

 /*
  * 関数名　　　　：PlayAnimation()
  * 処理内容　　　：プレイヤーのアニメーション再生(再生中は操作不可)
  * 戻り値　　　　：なし
  */
 void APlayerBase::PlayAnimation(UAnimMontage* _toPlayAnimMontage, FName _startSectionName /*= "None"*/, float _playRate /*= 1.f*/)
 {
	 // 攻撃不可に
	 CanCombo = false;

	 // 再生するアニメーションを格納
	 UAnimMontage* toPlayAnimMontage = _toPlayAnimMontage;

	 // アニメーション再生
	 if (toPlayAnimMontage != nullptr)
	 {
		 PlayAnimMontage(_toPlayAnimMontage, _playRate, _startSectionName);
	 }
 }


