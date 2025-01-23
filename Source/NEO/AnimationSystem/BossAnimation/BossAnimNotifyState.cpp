// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnimNotifyState.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

#include "NEO/CharacterSystem/BossSystem/OdaBase.h"


// í ímíÜèÌÇ…Ç…çsÇ§èàóù
void UBossAnimNotifyState::NotifyTickAction(AActor* _Owner, UAnimSequenceBase* Animation, float FrameDeltaTime)
{

	if (NotifyType == EBossNotifyStateType::NotifyType_RootMotion_BossAttack1)
	{
		RootMotion(_Owner, 15.f);
	}

	if (NotifyType == EBossNotifyStateType::NotifyType_BossAttack1)
	{
		//BossGet
		AOdaBase* Oda = Cast<AOdaBase>(_Owner);
		if (Oda) {
			Oda->SetCollision();
		}
	}
}
