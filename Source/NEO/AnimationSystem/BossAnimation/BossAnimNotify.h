// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NEO/AnimationSystem/CharacterAnimNotify.h"
#include "BossAnimNotify.generated.h"

UENUM(BlueprintType)
enum class EBossNotifyType :uint8
{
	NotifyType_BossAttack2				UMETA(DisplayName = "BossAttack2"),
	NotifyType_BossAttack2Conbo			UMETA(DisplayName = "BossAttack2_Conbo"),
	NotifyType_BossAttacked				UMETA(DisplayName = "BossAttacked"),
	NotifyType_BossDeath				UMETA(DisplayName = "BossDeath"),
	NotifyType_BossHPLock				UMETA(DisplayName = "BossHPLock"),
	NotifyType_BossUlt					UMETA(DisplayName = "BossUlt")

};

UCLASS()
class NEO_API UBossAnimNotify : public UCharacterAnimNotify
{
	GENERATED_BODY()

public:
	virtual bool CanBePlaced(UAnimSequenceBase* Animation) const { return true; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
		EBossNotifyType NotifyType;

	//•KŽE‹Z‚ÌŽí—Þ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ultimate")
		TSubclassOf<AActor>UltInfo;


	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

};
