// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NEO/AnimationSystem/CharacterAnimNotifyState.h"
#include "PlayerAnimNotifyState.generated.h"


UENUM(BlueprintType)
enum class EPlayerNotifyStateType :uint8
{
    NotifyType_RootMotion_Attack1   UMETA(DisplayName = "RootMotion_Attack1"),
    NotifyType_RootMotion_Attack2   UMETA(DisplayName = "RootMotion_Attack2"),
    NotifyType_RootMotion_Attack3   UMETA(DisplayName = "RootMotion_Attack3"),
    NotifyType_RootMotion_KnockBack UMETA(DisplayName = "RootMotion_KnockBack"),
};

UCLASS()
class NEO_API UPlayerAnimNotifyState : public UCharacterAnimNotifyState
{
	GENERATED_BODY()
	
        // 通知をエディタに公開するかどうか
        virtual bool CanBePlaced(UAnimSequenceBase* Animation) const { return true; }


protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
        EPlayerNotifyStateType NotifyType;

    // 通知開始時に行う処理
    virtual void NotifyBeginAction(AActor* _Owner, UAnimSequenceBase* Animation, float TotalDuration);

    // 通知中常にに行う処理
    virtual void NotifyTickAction(AActor* _Owner, UAnimSequenceBase* Animation, float FrameDeltaTime);

    // 通知終了時に行う処理
    virtual void NotifyEndAction(AActor* _Owner, UAnimSequenceBase* Animation);
};
