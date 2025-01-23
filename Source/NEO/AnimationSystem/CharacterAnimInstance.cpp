// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

UCharacterAnimInstance::UCharacterAnimInstance()
    : WeaponType(EWeaponType::WeaponType_None)
{
}

void UCharacterAnimInstance::NativeBeginPlay()
{
    // オーナーを取得する
    OwnerCharacter = Cast<ACharacter>(GetOwningActor());

    // キャラクタームーブメント取得
    if (OwnerCharacter)
    {
        CharacterMovementComp = OwnerCharacter->GetCharacterMovement();
    }
}


//
void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // OwnerCharacterの現在のステータス取得
    if (OwnerCharacter)
    {
        // 速度を取得する
        Velocity = OwnerCharacter->GetVelocity();

        // 移動スピード取得
        GroundSpeed = Velocity.Size();

        // 
        ShouldMove = (CharacterMovementComp->GetCurrentAcceleration() != FVector::ZeroVector) && (GroundSpeed > 3.f);
        
        // 空中にいるかどうかを取得する
        IsFall = CharacterMovementComp->IsFalling();
    }
}

