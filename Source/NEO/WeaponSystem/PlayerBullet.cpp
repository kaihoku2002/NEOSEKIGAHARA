// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBullet.h"
#include "NEO/CharacterSystem/Enemy/EnamyBase.h"
#include "NEO/CharacterSystem/CharacterBase.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APlayerBullet::APlayerBullet()
	: MoveSpeed(20.f)
	, DamageAmount(50)
	, TimeToDelete(5.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 対応するオブジェクト生成
	BulletCollision = CreateDefaultSubobject<UCapsuleComponent>("BulletCollision");

	// ルートに設定
	if (BulletCollision)
	{
		// 武器のメッシュに追従
		BulletCollision->SetupAttachment(RootComponent);
	}

	// 武器のコンポーネントを作成
	BulletStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletStaticMesh");

	if (BulletStaticMesh)
	{
		// 弾丸のアセット設定
		ConstructorHelpers::FObjectFinder< UStaticMesh > weaponMesh(TEXT("/Game/0122/Player/Weapon/Cartrige/Cartrige"));

		if (weaponMesh.Succeeded())
		{
			BulletStaticMesh->SetStaticMesh(weaponMesh.Object);
			BulletStaticMesh->SetupAttachment(BulletCollision);
		}
	}


	// コリジョンイベントを設定
	BulletCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerBullet::OnOverlap);
}

// Called when the game starts or when spawned
void APlayerBullet::BeginPlay()
{
	Super::BeginPlay();
	
	// 指定の時間で弾丸を削除
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(DeleteTimerHandle, this, &APlayerBullet::SetDestroyFlag, TimeToDelete, false);
}

// Called every frame
void APlayerBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 常に前進
	Move();
}


void APlayerBullet::Move()
{
	// 移動する量
	const FVector MoveValue = FVector(MoveSpeed, 0.f, 0.f);

	// 移動値反映
	AddActorLocalOffset(MoveValue);
}


// 削除までの時間に達していたら
void APlayerBullet::SetDestroyFlag()
{
	// Tick処理を切る
	PrimaryActorTick.bCanEverTick = false;

	// 削除の時間に達したことを伝えるためのフラグ
	const bool DestroyFlag = true;

	// 削除
	DestroyBullet(DestroyFlag);
}

// 弾のDestroy
void APlayerBullet::DestroyBullet(bool _destroyFlag /*= false*/)
{
	// 削除通知が来た時削除
	if (_destroyFlag)
	{
		Destroy();
	}
}


void APlayerBullet::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// オーバーラップした際に実行したいイベント
	if (OtherActor && (OtherActor != this))
	{
		// 当たったのがEnemyの時ダメージ処理を行う
		if (OtherActor->ActorHasTag("Enemy"))
		{
			ACharacterBase* HitEnemy = Cast<ACharacterBase>(OtherActor);

			if (HitEnemy)
			{
				HitEnemy->TakedDamage(DamageAmount);
			}

			// 弾丸削除
			DestroyBullet(true);
		}
	}
}