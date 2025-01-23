// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NEO/AnimationSystem/CharacterAnimNotifyState.h"
#include "EnemyAnimNotifyState.generated.h"


UENUM(BlueprintType)
enum class EEnemyNotifyType2 :uint8
{
    NotifyType1 UMETA(DisplayName = "SwitchEnemyCollision"),
    NotifyType2 UMETA(DisplayName = "SpawnEffect"),
};

UCLASS()
class NEO_API UEnemyAnimNotifyState : public UCharacterAnimNotifyState
{
	GENERATED_BODY()
	
    // 通知をエディタに公開するかどうか
    virtual bool CanBePlaced(UAnimSequenceBase* Animation) const { return true; }


protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
        EEnemyNotifyType2 NotifyType;

    // 通知開始時に行う処理
    virtual void NotifyBeginAction(AActor* _Owner, UAnimSequenceBase* Animation, float TotalDuration);

    // 通知終了時に行う処理
    virtual void NotifyEndAction(AActor* _Owner, UAnimSequenceBase* Animation);
};
