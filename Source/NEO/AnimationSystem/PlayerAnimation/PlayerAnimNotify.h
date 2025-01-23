// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NEO/AnimationSystem/CharacterAnimNotify.h"
#include "PlayerAnimNotify.generated.h"

UENUM(BlueprintType)
enum class EPlayerNotifyType :uint8
{
    NotifyType_SetCollision UMETA(DisplayName = "SetCollision"),
    NotifyType_ResetCombo   UMETA(DisplayName = "ResetCombo"),
    NotifyType_SaveCombo    UMETA(DisplayName = "SaveCombo"),
    NotifyType_CanControl   UMETA(DisplayName = "CanControl"),
    NotifyType_SlowDown     UMETA(DisplayName = "SlowDown"),
    NotifyType_DropWeapon   UMETA(DisplayName = "DropWeapon"),
    NotifyType_StopMontage   UMETA(DisplayName = "StopMontage"),
};


UCLASS()
class NEO_API UPlayerAnimNotify : public UCharacterAnimNotify
{
    GENERATED_BODY()

        
public:

    // 通知をエディタに公開するかどうか
    virtual bool CanBePlaced(UAnimSequenceBase* Animation) const;

protected:

    // 通知名を管理するEnum
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "AnimNotify")
        EPlayerNotifyType NotifyType;

    // 通知が来た時の処理
    virtual void NotifyAction(AActor* _Owner,UAnimSequenceBase* Animation);
};