// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CharacterAnimNotify.generated.h"

UCLASS()
class NEO_API UCharacterAnimNotify : public UAnimNotify
{
    GENERATED_BODY()


public:

    // 通知が来たときに行う処理
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

    // 通知をエディタに公開するかどうか
    virtual bool CanBePlaced(UAnimSequenceBase* Animation) const { return false; }


protected:

    // 通知が来た時に行う処理(派生先のクラスでオーバーライド)
    virtual void NotifyAction(AActor* _Owner, UAnimSequenceBase* Animation) { return; }

private:

    // オーナーのアクターを取得
    AActor* GetOwnerActor(USkeletalMeshComponent* MeshComp);
};

