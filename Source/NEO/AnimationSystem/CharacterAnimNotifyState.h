// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CharacterAnimNotifyState.generated.h"

UCLASS()
class NEO_API UCharacterAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:

    // 通知開始時の処理
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);

    // 通知中常に処理
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime);

    // 通知終了時の処理
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

    // 通知をエディタに公開するかどうか
    virtual bool CanBePlaced(UAnimSequenceBase* Animation) const { return false; }


protected:

    // 通知開始時に行う処理(派生先のクラスでオーバーライド)
    virtual void NotifyBeginAction(AActor* _Owner, UAnimSequenceBase* Animation, float TotalDuration) { return; }

    // 通知中常に行う処理(派生先のクラスでオーバーライド)
    virtual void NotifyTickAction(AActor* _Owner,UAnimSequenceBase* Animation, float FrameDeltaTime) { return; }

    // 通知終了時に行う処理(派生先のクラスでオーバーライド)
    virtual void NotifyEndAction(AActor* _Owner,UAnimSequenceBase* Animation) { return; }

    // 疑似的にルートモーションを再現
    virtual void RootMotion(AActor* _pOwner,float _moveValue);

private:

    // オーナーのアクターを取得
    AActor* GetOwnerActor(USkeletalMeshComponent* MeshComp);	
};
