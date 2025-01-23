// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimNotifyState.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "NEO/CharacterSystem/Enemy/EnamyBase.h"

void UEnemyAnimNotifyState::NotifyBeginAction(AActor* _Owner, UAnimSequenceBase* Animation, float TotalDuration)
{
    AEnamyBase* pEnemy = Cast<AEnamyBase>(_Owner);


    if (pEnemy)
    {
        if (NotifyType == EEnemyNotifyType2::NotifyType1)
        {
            pEnemy->CollisionOn();
        }
    }
}

void UEnemyAnimNotifyState::NotifyEndAction(AActor* _Owner, UAnimSequenceBase* Animation)
{
    AEnamyBase* pEnemy = Cast<AEnamyBase>(_Owner);


    
}




