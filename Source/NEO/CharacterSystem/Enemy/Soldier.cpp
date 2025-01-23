#include "Soldier.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "NEO/WeaponSystem/WeaponBase.h"

ASoldier::ASoldier()
{
    PrimaryActorTick.bCanEverTick = true;

    ComboCounter = 0;
    AttackCooldown = 1.0f;
    LastAttackTime = -AttackCooldown;
    bIsAttacking = false;
    bIsOnCooldown = false;
    IsIdol = false;
    IsRunning = true;
}
/*
 * 関数名　　　　：ASoldier::BeginPlay
 * 処理内容　　　：初期設定を行います。武器のスポーンなど。
 * 戻り値　　　　：なし（void）
 */
void ASoldier::BeginPlay()
{
    Super::BeginPlay();

}
/*
 * 関数名　　　　：ASoldier::AttackCombo
 * 処理内容　　　：コンボ攻撃のロジックを実行します。
 * 戻り値　　　　：なし（void）
 */
void ASoldier::AttackCombo()
{
    if (bIsOnCooldown) return;
    ComboCounter++;
    if (ComboCounter == 1)
    {
        PlayAnimMontage(Attack, 1, NAME_None);
        Damage = 5.f;
    }
    else if (ComboCounter == 2)
    {
        PlayAnimMontage(Attack2, 1, NAME_None);
        Damage = 10.f;
    }
    else if (ComboCounter >= 3)
    {
        PlayAnimMontage(Attack3, 1, NAME_None);
        bIsOnCooldown = true;
        GetWorld()->GetTimerManager().SetTimer(ComboCooldownTimer, this, &ASoldier::EndComboCooldown, 1.5f, false);
        Damage = 15.f;
        ComboCounter = 0;
    }

    GetWorld()->GetTimerManager().ClearTimer(ComboResetTimer);

    GetWorld()->GetTimerManager().SetTimer(ComboResetTimer, this, &ASoldier::ResetCombo, 2.0f, false);
    IsRunning = false;
    bIsAttacking = true;
}
/*
 * 関数名　　　　：ASoldier::ResetCombo
 * 処理内容　　　：コンボカウンターをリセットし、攻撃状態を更新します。
 * 戻り値　　　　：なし（void）
 */
void ASoldier::ResetCombo()
{
    ComboCounter = 0;
    bIsAttacking = false;
    IsRunning = true;
    IsIdol = false;
}
/*
 * 関数名　　　　：ASoldier::CollisionOn
 * 処理内容　　　：武器のコリジョンを有効にします。
 * 戻り値　　　　：なし（void）
 */
void ASoldier::CollisionOn()
{
    if (GetWeapon())
    {
        SetCollision();
    }
}
/*
 * 関数名　　　　：ASoldier::Tick
 * 処理内容　　　：毎フレーム呼び出され、キャラクターの状態や位置を更新します。
 * 戻り値　　　　：なし（void）
 */
void ASoldier::Tick(float DeltaTime)
{
    if (bIsNowDamage )
    {
       
        return;
    }
    Super::Tick(DeltaTime);
    if (PlayerCharacter)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector EnemyLocation = GetActorLocation();
        float DistanceOnY = FMath::Abs(PlayerLocation.Y - EnemyLocation.Y);
        if(DistanceOnY <= 100.0f && GetWorld()->GetTimeSeconds() - LastAttackTime >= AttackCooldown&&Health>0)
        {
            AttackCombo();
            LastAttackTime = GetWorld()->GetTimeSeconds();
        }
    }
    PlayerCharacter = Cast<ACharacter>(GetPlayer());
    if (!PlayerCharacter) return;

    float CurrentDistance = GetDistanceToPlayer();
    FVector DirectionToPlayer = GetPlayerDirection();
    FVector SnappedDirection;
    FVector MoveVector;

    if (Health > 0 && bIsAttacking == false)
    {
        if (CurrentDistance > DesiredDistance)
        {
            SnappedDirection = GetSnappedDirection(DirectionToPlayer);
            MoveVector = SnappedDirection * MoveSpeed * DeltaTime;
            
        }
       
        else
        {
            return; // その他の場合は移動しない
        }
    }
    SetActorLocation(GetActorLocation() + MoveVector);
    if (Health <= 0)
    {
        if (GetWeapon())
        {
            DetachWeapon(false);
        }
    }
}
/*
 * 関数名　　　　：ASoldier::GetSnappedDirection
 * 処理内容　　　：与えられた方向ベクトルを調整します。
 * 戻り値　　　　：調整された方向ベクトル（FVector）
 */
FVector ASoldier::GetSnappedDirection(const FVector& Direction) const
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


void ASoldier::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}
/*
 * 関数名　　　　：ASoldier::GetPlayerDirection
 * 処理内容　　　：プレイヤーへの方向ベクトルを取得します。
 * 戻り値　　　　：プレイヤーへの方向ベクトル（FVector）
 */

FVector ASoldier::GetPlayerDirection() const
{
    if (!PlayerCharacter) return FVector::ZeroVector;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector LancerLocation = GetActorLocation();
    return FVector(PlayerLocation.X - LancerLocation.X, PlayerLocation.Y - LancerLocation.Y, 0.0f).GetSafeNormal();
}
/*
 * 関数名　　　　：ASoldier::GetDistanceToPlayer
 * 処理内容　　　：プレイヤーまでの距離を取得します。
 * 戻り値　　　　：プレイヤーまでの距離（float）
 */
float ASoldier::GetDistanceToPlayer() const
{
    if (!PlayerCharacter) return 0.0f;

    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector LancerLocation = GetActorLocation();
    return FVector::Distance(PlayerLocation, LancerLocation);
}

/*
 * 関数名　　　　：ASoldier::EndComboCooldown
 * 処理内容　　　：コンボのクールダウンを終了し、キャラクターの状態を更新します。
 * 戻り値　　　　：なし（void）
 */
void ASoldier::EndComboCooldown()
{
    bIsOnCooldown = false;
    IsRunning = true;
    IsIdol = false;
}





