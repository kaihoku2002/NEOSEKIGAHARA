// Fill out your copyright notice in the Description page of Project Settings.


#include "EnamyBase.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h" 
#include "Camera/CameraComponent.h"
#include "NEO/GameSystem/NEOGameMode.h"
#include "NEO/GameSystem/GameSystem_BattleArea.h"
#include "NEO/CharacterSystem/ActionAssistComponent.h"
#include "NEO/CharacterSystem/PlayerSystem/PlayerBase.h"


// Sets default values
AEnamyBase::AEnamyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//UI Create

	bIsDeath = false;

	Damage = 5.0f;

	bHasPattern1Tag = Tags.Contains("pattern1");
	bHasPattern2Tag = Tags.Contains("pattern2");
	bHasPattern3Tag = Tags.Contains("pattern3");
	bHasWeponTag = Tags.Contains("HasWepon");



	MoveSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MoveSpline"));
	MoveSpline->SetupAttachment(RootComponent);
	/*MovementSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MovementSpline1"));
	MovementSpline->SetupAttachment(RootComponent);*/
}
/*
 * 関数名　　　　：AEnemyBase::DestroyEnemy
 * 処理内容　　　：敵キャラクターを破壊し、ゲームモードのDestroyEnemyメソッドを呼び出します。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::DestoryEnemy()
{
	ANEOGameMode* GameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->DestroyEnemy(this, IsAreaEnemy);

	}
	
}

/*
 * 関数名　　　　：AEnemyBase::BeginPlay
 * 処理内容　　　：敵キャラクターの初期化とバトルエリアの取得を行います。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnDefaultController();

	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}
	TArray<AActor*> TempBattleAreas;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameSystem_BattleArea::StaticClass(), TempBattleAreas);

	for (AActor* Actor : TempBattleAreas) {
		AGameSystem_BattleArea* BattleArea = Cast<AGameSystem_BattleArea>(Actor);
		if (BattleArea) {

		}
	}
	
	//bGetAway = UKismetMathLibrary::RandomBool();
	bGetAway = false;
}
/*
 * 関数名　　　　：AEnemyBase::GetPlayer
 * 処理内容　　　：プレイヤーポーンを取得して返します。
 * 戻り値　　　　：プレイヤーポーンのポインタ（AActor*）
 */
AActor* AEnamyBase::GetPlayer()
{
	APawn* PlayerChara = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	return PlayerChara;
}

/*
 * 関数名　　　　：AEnemyBase::Tick
 * 処理内容　　　：毎フレーム呼び出され、キャラクターの状態や位置を更新します。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsNowDamage)
	{
		return;
	}


	//キャラクターの位置を取得
	FVector CharacterLocation = GetActorLocation();

	// 自分の座標を取得
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			APawn* Pawn = PlayerController->GetPawn();
			if (Pawn)
			{
				FVector MyLocation = Pawn->GetActorLocation();
				// キャラクターの位置と自分の位置を比較してY軸より前にいるかどうかを判定
				bIsRotation = CharacterLocation.Y > MyLocation.Y;
				bIsRotationTag2 = CharacterLocation.X > MyLocation.X;
				//bIsRotationがtrueなら
				if (Health >= 0)
				{

					bool LookRight = false;

					if (ActorHasTag("pattern1"))
					{
						if (bIsRotation)
						{
							
							LookRight = true;
						}
						else
						{
							
							LookRight = false;
						}

						ActionAssistComp->OwnerParallelToCamera(LookRight);
					}
					else if (ActorHasTag("pattern2"))
					{
						if (bIsRotationTag2)
						{
						
							LookRight = true;

						}
						else
						{
							LookRight = false;
						}

						ActionAssistComp->OwnerParallelToCamera(LookRight);
					}
					else if (ActorHasTag("pattern3"))
					{
						if (bIsRotation)
						{
					
							LookRight = false;

						}
						else
						{

							LookRight = true;
						}

						ActionAssistComp->OwnerParallelToCamera(LookRight);
					}
				}

			}
		}
	}

	if (bHasPattern1Tag && GetWorld()->GetTimeSeconds() - SpawnTime < 4.5f || bHasPattern2Tag || bHasPattern3Tag)
	{
		float TimeSinceSpawn = GetWorld()->GetTimeSeconds() - SpawnTime;
		float SplineDuration = 3.0f;  // スプラインを完了するまでの時間
		float SplineProgress = FMath::Clamp(TimeSinceSpawn / SplineDuration, 0.0f, 1.0f);
		FVector NewLocation = MoveSpline->GetLocationAtSplineInputKey(SplineProgress, ESplineCoordinateSpace::World);
		SetActorLocation(NewLocation);
		for (AGameSystem_BattleArea* BattleArea : BattleAreaReferences) {
			if (BattleArea && BattleArea->IsOverlappingActor(this)) {
				BattleArea->IgnoreCollision();
			}
		}
		return; // 他のTick処理をスキップ
	}
	else
	{
		for (AGameSystem_BattleArea* BattleArea : BattleAreaReferences) {
			if (BattleArea) {
				BattleArea->ResetCollision();
			}
		}
	}
	if (bShouldMoveAlongSpline) {
		// 3秒間移動させる
		TimeSinceStartOfMovement += DeltaTime;
		float Alpha = FMath::Min(TimeSinceStartOfMovement / MovementDuration, 1.0f);
		FVector NewLocation = MovementSpline->GetLocationAtDistanceAlongSpline(MovementSpline->GetSplineLength() * Alpha, ESplineCoordinateSpace::Local);
		SetActorLocation(NewLocation);

		if (Alpha >= 1.0f) {
			bShouldMoveAlongSpline = false;
		}

		// 他のTick処理を停止する
		return;
	}

	CheckHealth();
	
}



/*
 * 関数名　　　　：AEnemyBase::ApplyDamage
 * 処理内容　　　：ダメージを適用し、キャラクターの状態を更新します。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::TakedDamage(float DamageAmount, bool _bLastAttack)
{

	Health -= DamageAmount;
	FTimerHandle TimerHandle_ResetDamage;
	bIsNowDamage = true;
	GetWorldTimerManager().SetTimer(TimerHandle_ResetDamage, this, &AEnamyBase::DamageReac, 0.2f, false);
	ActionAssistComp->SpawnEffect(NiagaraEffect, GetActorLocation());
	ActionAssistComp->HitStop(0.01f,0.1f);
	if (Health <= 0&& bGetAway == false)
	{

		PlayAnimMontage(Death, 0.8, NAME_None);
		GetWorldTimerManager().SetTimer(TimerHandle_DestroyEnemy, this, &AEnamyBase::AfterDeath, 1.6f, true);
		bIsDeath = true;
		FVector ForceDirection = -GetActorForwardVector();
		float ForceStrength = 1000.0f;  // Adjust this value as needed.
		Tags.Remove(FName("Enemy"));

		GetCharacterMovement()->AddForce(ForceDirection * ForceStrength);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);


	}
	else
	{
		PlayAnimMontage(Damage_Reaction, 0.8, NAME_None);


	}


}


void AEnamyBase::Attack_Sword(TTuple<TArray<AActor*>, TArray<FVector>> HitActorAndLocation)
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
				Cast<ACharacterBase>(ActorArray)->TakedDamage(10.f, false);
				break;
			}
		}
	}
}

void AEnamyBase::Attack_Lance(TTuple<TArray<AActor*>, TArray<FVector>> HitActorAndLocation)
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
				Cast<ACharacterBase>(ActorArray)->TakedDamage(10.f, false);
				break;
			}
		}
	}
}



/*
 * 関数名　　　　：AEnemyBase::MaintainDistanceFromEnemy
 * 処理内容　　　：敵キャラクターから一定の距離を保つように移動します。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::MaintainDistanceFromEnemy()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* CurrentActor = *It;
		if (CurrentActor && CurrentActor->ActorHasTag(FName("Enemy")))
		{
			FVector Direction = CurrentActor->GetActorLocation() - GetActorLocation();
			float Distance = Direction.Size();

			if (Distance < DesiredDistanceFromEnemy)
			{
				FVector MoveDirection = -Direction.GetSafeNormal() * (DesiredDistanceFromEnemy - Distance);
				AddMovementInput(MoveDirection);
			}
		}
	}
}
/*
 * 関数名　　　　：AEnemyBase::AfterDeath
 * 処理内容　　　：キャラクターの死後の処理を行います。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::AfterDeath()
{
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	DestoryEnemy();

}
/*
 * 関数名　　　　：AEnemyBase::DamageReac
 * 処理内容　　　：ダメージ反応のフラグをリセットします。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::DamageReac()
{
	bIsNowDamage = false;
}
/*
 * 関数名　　　　：AEnemyBase::CheckHealth
 * 処理内容　　　：キャラクターの健康状態を確認し、必要に応じて死亡トリガーを生成します。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::CheckHealth()
{
	if (Health <= 0)
	{
		SpawnDeathTrigger();
	}
}
/*
 * 関数名　　　　：AEnemyBase::SpawnDeathTrigger
 * 処理内容　　　：死亡トリガーを生成します。
 * 戻り値　　　　：なし（void）
 */
void AEnamyBase::SpawnDeathTrigger()
{
	if (DeathTriggerClass = nullptr)
	{
		FTransform SpawnTransform = GetActorTransform();
		ADeathTrigger* DeathTrigger = GetWorld()->SpawnActor<ADeathTrigger>(DeathTriggerClass, SpawnTransform);
		if (DeathTriggerClass != nullptr)
		{
			DeathTrigger->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}


/*
 * 関数名　　　　：AEnemyBase::GetEnemyActor
 * 処理内容　　　：Enemyタグを持つアクターを取得します。
 * 戻り値　　　　：Enemyタグを持つ最初のアクター。見つからない場合は nullptr。
 */
AActor* AEnamyBase::GetEnemyActor() const
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->ActorHasTag(FName("Enemy")))
		{
			return Actor; // Enemyタグを持つ最初のアクターを返す
		}
	}
	return nullptr; // Enemyタグを持つアクターが見つからない場合
}

void AEnamyBase::OnOverlapBeginWall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetCapsuleComponent())  // WallCollision と同じ
	{
		// この EnemyBase のコリジョンレスポンスを Block に設定
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	}
}




