// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "PlayerBullet.h"


// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// 武器のメッシュのパス
	TCHAR* WeaponAssetPath = TEXT("/Game/0122/Player/Weapon/Gun/Gun");

	// 武器のメッシュ設定
	SetupWeaponMesh(WeaponStaticMesh, WeaponAssetPath);

	// コリジョン設定
	SetupCollisionComponent(WeaponCollision);

	// 武器の種類を刀に設定
	WeaponType = EWeaponType::WeaponType_Gun;
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*
 * 関数名　　　　：Shoot()
 * 処理内容　　　：射撃処理
 * 引数１　　　　：弾丸の攻撃力
 * 戻り値　　　　：なし
 */
void AGun::Shoot(int32 _damageAmount /*= 0*/)
{
	// 銃弾をスポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// スポーン時の場所と回転を指定
	FVector SpawnBulletLocation = GetActorLocation();

	FRotator SpawnBulletRotation = pOwner->GetActorRotation();

	// 発射
	APlayerBullet* Bullet = GetWorld()->SpawnActor<APlayerBullet>(BulletClass, SpawnBulletLocation, SpawnBulletRotation, SpawnParams);

	// 新しく攻撃力が設定されていなければ元の攻撃力を参照
	int32 DamageAmount = (_damageAmount != 0) ? (_damageAmount) : (GetDamage());

	// 攻撃力設定
	Bullet->SetDamageAmount(DamageAmount);

	if (ShootSoundObj)
	{
		// 発射音再生
		//ActionAssistComp->PlaySound(ShootSoundObj);
	}
}