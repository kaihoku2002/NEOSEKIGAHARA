// Fill out your copyright notice in the Description page of Project Settings.


#include "Lance.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ALance::ALance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// 武器のメッシュのパス
	TCHAR* WeaponAssetPath = TEXT("/Game/0122/Player/Weapon/Lance/Lance");

	// 武器のメッシュ設定
	SetupWeaponMesh(WeaponStaticMesh, WeaponAssetPath);

	// コリジョン設定
	SetupCollisionComponent(WeaponCollision);

	// 武器の種類を刀に設定
	WeaponType = EWeaponType::WeaponType_Lance;
}

// Called when the game starts or when spawned
void ALance::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void ALance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
