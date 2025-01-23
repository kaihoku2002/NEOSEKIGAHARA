// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NEO/AnimationSystem/CharacterAnimNotifyState.h"
#include "BossAnimNotifyState.generated.h"

UENUM(BlueprintType)
enum class EBossNotifyStateType :uint8
{
    NotifyType_RootMotion_BossAttack1   UMETA(DisplayName = "RootMotion_BossAttack1"),
    NotifyType_BossAttack1              UMETA(DisplayName = "BossAttack1"),
};

UCLASS()
class NEO_API UBossAnimNotifyState : public UCharacterAnimNotifyState
{
    GENERATED_BODY()

     virtual bool CanBePlaced(UAnimSequenceBase* Animation) const { return true; }

public:
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
     EBossNotifyStateType NotifyType;

    // 通知開始時に行う処理
     virtual void NotifyBeginAction(AActor* _Owner, UAnimSequenceBase* Animation, float TotalDuration) { return; }

    // 通知中常にに行う処理
     virtual void NotifyTickAction(AActor* _Owner, UAnimSequenceBase* Animation, float FrameDeltaTime);

    // 通知終了時に行う処理
    virtual void NotifyEndAction(AActor* _Owner, UAnimSequenceBase* Animation) { return; }

};
