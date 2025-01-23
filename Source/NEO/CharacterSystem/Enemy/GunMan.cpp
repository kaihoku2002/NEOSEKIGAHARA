// Fill out your copyright notice in the Description page of Project Settings.


#include "GunMan.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NEO/CharacterSystem/PlayerSystem\PlayerCharacter.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Bullet.h"
#include "TrajectoryBullet.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NEO/BackGroundSystem/GunFence.h"
#include "NavigationPath.h"
#include "Kismet/KismetMathLibrary.h"
#include "NEO/WeaponSystem/WeaponBase.h"


// Sets default values
AGunMan::AGunMan()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;


    MaxHealth = 60;
    Health = MaxHealth;
    bIsBulletAlive = false;
    bCanMove = true;
}


/*
 * 関数名　　　　：AGunMan::BeginPlay
 * 処理内容　　　：初期設定を行います。タイマーの設定、武器のスポーンなど。
 * 戻り値　　　　：なし（void）
 */void AGunMan::BeginPlay()
{
    Super::BeginPlay();
    // プレイヤーキャラクターの参照を取得
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AGunMan::PlayAttackAnim, 5.0f, true);

}
 /*
 * 関数名　　　　：AGunMan::GetSnappedDirection
 * 処理内容　　　：与えられた方向ベクトルを調整します。
 * 戻り値　　　　：調整された方向ベクトル（FVector）
 */
FVector AGunMan::GetSnappedDirection(const FVector& Direction) const
{
    FVector SnappedDirection = Direction;

    if (FMath::Abs(SnappedDirection.X) > FMath::Abs(SnappedDirection.Y))
    {
        SnappedDirection.Y = 0.0f;
    }
    else
    {
        SnappedDirection.X = 0.0f;
    }

    return SnappedDirection.GetSafeNormal();
}
/*
 * 関数名　　　　：AGunMan::CollisionOn
 * 処理内容　　　：武器のコリジョンを有効にします。
 * 戻り値　　　　：なし（void）
 */

void AGunMan::CollisionOn()
{
    if (GetWeapon())
    {
        SetCollision();
    }
}
/*
 * 関数名　　　　：AGunMan::Tick
 * 処理内容　　　：毎フレーム呼び出され、キャラクターの状態や位置を更新します。
 * 戻り値　　　　：なし（void）
 */
void AGunMan::Tick(float DeltaTime)
{

    Super::Tick(DeltaTime);
    if (Health <= 0)
    {
        if (GetWeapon())
        {
            DetachWeapon(false);
        }
    }
  

    if (bIsNowDamage)
    {
        return;
    }
    PlayerCharacter = Cast<ACharacter>(GetPlayer());
    if (!PlayerCharacter) return;

    float CurrentDistance = GetDistanceToPlayer();
    FVector DirectionToPlayer = GetPlayerDirection();
    FVector SnappedDirection;
    FVector MoveVector;

    if (bCanMove == true)
    {
        if (CurrentDistance > DesiredDistance)
        {
            SnappedDirection = GetSnappedDirection(DirectionToPlayer);
            MoveVector = SnappedDirection * MoveSpeed * DeltaTime;
            IsRunning = true;
            IsIdol = false;
        }
        else if (CurrentDistance < DesiredDistance + 400)
        {
            SnappedDirection = GetSnappedDirection(-DirectionToPlayer);
            MoveVector = SnappedDirection * MoveSpeed / 3 * DeltaTime;
            IsRunning = true;
            IsIdol = false;
        }
        else
        {
            return; // その他の場合は移動しない
        }

        SetActorLocation(GetActorLocation() + MoveVector);
    }
    FVector Start = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector End = ((ForwardVector * 15.f) + Start);  // 200 はトレースの距離、適宜調整すること

    FHitResult HitResult;

    // トレースの設定
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);  // 自分自身は無視する

    // Line Trace 実行
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

    // ヒットした場合
    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor && HitActor->Tags.Contains("Fence"))
        {
            // Fence タグを持つオブジェクトが目の前にある！
            // 何か処理をする
            IsRunning = false;
            IsIdol = true;
            bCanMove = false;
            MoveSpeed = 0;
        }
       
    }
    else
    {
        IsRunning = true;
        IsIdol = false;
        bCanMove = true;
        MoveSpeed = 100;
    }
}

/*
 * 関数名　　　　：AGunMan::GetPlayerDirection
 * 処理内容　　　：プレイヤーへの方向ベクトルを取得します。
 * 戻り値　　　　：プレイヤーへの方向ベクトル（FVector）
 */

FVector AGunMan::GetPlayerDirection() const
{
    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector GunManLocation = GetActorLocation();
    return FVector(PlayerLocation.X - GunManLocation.X, PlayerLocation.Y - GunManLocation.Y, 0.0f).GetSafeNormal();
}
/*
 * 関数名　　　　：AGunMan::GetDistanceToPlayer
 * 処理内容　　　：プレイヤーまでの距離を取得します。
 * 戻り値　　　　：プレイヤーまでの距離（float）
 */
float AGunMan::GetDistanceToPlayer() const
{
    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector GunManLocation = GetActorLocation();
    return FVector::Distance(PlayerLocation, GunManLocation);
}
/*
 * 関数名　　　　：AGunMan::SpawnTrajectoryBullet
 * 処理内容　　　：トラジェクトリーバレットをスポーンします。
 * 戻り値　　　　：なし（void）
 */
void AGunMan::SpawnTrajectoryBullet()
{


    USkeletalMeshComponent* CharacterMesh = GetMesh();
    FVector SpawnLocation = CharacterMesh->GetSocketLocation("enemy_root");
    FRotator SpawnRotation = CharacterMesh->GetSocketRotation("enemy_root");
    if (TrajectoryBulletClass)
    {
        AActor* SpawnedTrajectoryBullet = GetWorld()->SpawnActor<AActor>(TrajectoryBulletClass, SpawnLocation, SpawnRotation);
        if (SpawnedTrajectoryBullet)
        {
            SpawnedTrajectoryBullet->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "enemy_root");
        }
    }
}
/*
 * 関数名　　　　：AGunMan::ReplaceWithBullet
 * 処理内容　　　：バレットと置き換えます。
 * 戻り値　　　　：なし（void）
 */
void AGunMan::ReplaceWithBullet()
{
   
    FVector SpawnBulletLocation = GetActorLocation(); // または他の場所
    FRotator SpawnBulletRotation = GetActorRotation(); // または他の回転
    if (BulletClass)
    {
        AActor* SpawnedBullet = GetWorld()->SpawnActor<AActor>(BulletClass, SpawnBulletLocation, SpawnBulletRotation);


    }
}
/*
 * 関数名　　　　：AGunMan::ResumeMovement
 * 処理内容　　　：移動を再開します。
 * 戻り値　　　　：なし（void）
 */
void AGunMan::ResumeMovement()
{
    // Resume movement
    GetCharacterMovement()->Activate(true);

}
/*
 * 関数名　　　　：AGunMan::UnlockRotation
 * 処理内容　　　：回転をアンロックします。
 * 戻り値　　　　：なし（void）
 */
void AGunMan::UnlockRotation()
{
    bIsSpawningBullet = false;
}
/*
 * 関数名　　　　：AGunMan::PlayAttackAnim
 * 処理内容　　　：攻撃アニメーションを再生します。
 * 戻り値　　　　：なし（void）
 */
void AGunMan::PlayAttackAnim()
{
    if (Health > 0)
    {
        PlayAnimMontage(Attack, 1.0f, NAME_None);
        GetWorldTimerManager().SetTimer(TickEnableTimerHandle, this, &AGunMan::EnableTickAfterDelay, 3.0f, false);
        MoveSpeed = 0;
        IsRunning = false;
        IsIdol = true;
    }

}
/*
*関数名　　　　：AGunMan::EnableTickAfterDelay
* 処理内容　　　：ディレイ後にTickを有効化します。
* 戻り値　　　　：なし（void）
*/
void AGunMan::EnableTickAfterDelay()
{
    
    MoveSpeed = 100;
}