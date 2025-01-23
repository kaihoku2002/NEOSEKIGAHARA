// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimNotify.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"


/*
 * 関数名　　　　：Notify()
 * 処理内容　　　：通知が来た時に行う処理
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生中のアニメーション
 * 戻り値　　　　：なし
 */
void UCharacterAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// オーナー取得
	AActor* OwnerActor = GetOwnerActor(MeshComp);
	if (!OwnerActor) { return; }


	// 通知が来た時の処理
	NotifyAction(OwnerActor,Animation);
}


/*
 * 関数名　　　　：GetOwnerActor()
 * 処理内容　　　：オーナーをAActor*で取得
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 戻り値　　　　：オーナーをAActor*で取得
 */
AActor* UCharacterAnimNotify::GetOwnerActor(USkeletalMeshComponent* MeshComp)
{
	// プレイヤーのベースクラス取得
	AActor* pOwner = Cast<AActor>(MeshComp->GetOwner());
	if (!pOwner) { return nullptr; }

	return pOwner;
}