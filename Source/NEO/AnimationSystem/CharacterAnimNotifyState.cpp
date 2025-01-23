// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimNotifyState.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "NEO/CharacterSystem/CharacterBase.h"


/*
 * 関数名　　　　：NotifyBegin()
 * 処理内容　　　：通知開始時に行う処理
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生するアニメーション
 * 引数３　　　　：float TotalDuration・・・アニメーション再生総所要時間
 * 戻り値　　　　：なし
 */
void UCharacterAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	// オーナー取得
	AActor* OwnerActor = GetOwnerActor(MeshComp);
	if (!OwnerActor) { return; }


	// 通知開始時の処理
	NotifyBeginAction(OwnerActor, Animation,TotalDuration);
}


/*
 * 関数名　　　　：NotifyTick()
 * 処理内容　　　：通知中常に行う処理
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生中のアニメーション
 * 引数３　　　　：float FrameDeltaTime・・・1フレームの時間
 * 戻り値　　　　：なし
 */
void UCharacterAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	// オーナー取得
	AActor* OwnerActor = GetOwnerActor(MeshComp);
	if (!OwnerActor) { return; }


	// 通知中常に処理
	NotifyTickAction(OwnerActor,Animation,FrameDeltaTime);
}


/*
 * 関数名　　　　：NotifyEnd()
 * 処理内容　　　：通知終了時に行う処理
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 引数２　　　　：UAnimSequenceBase* Animation・・・再生していたアニメーション
 * 戻り値　　　　：なし
 */
void UCharacterAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// オーナー取得
	AActor* OwnerActor = GetOwnerActor(MeshComp);
	if (!OwnerActor) { return; }


	// 通知終了時の処理
	NotifyEndAction(OwnerActor,Animation);
}


/*
 * 関数名　　　　：GetOwnerActor()
 * 処理内容　　　：オーナーをAActor*で取得
 * 引数１　　　　：bool _isLookRight・・・向いている方向
 * 引数２　　　　：float _moveValue・・・毎フレーム移動値
 * 戻り値　　　　：なし
 */
void UCharacterAnimNotifyState::RootMotion(AActor* _pOwner,float _moveValue)
{
	if (!_pOwner) { return; }

	// 前方ベクトル取得
	FVector ForwardVector = _pOwner->GetActorForwardVector();

	// 現在向いている方向に移動する
	FVector newVector = ForwardVector * _moveValue;

	// キャラクターベースにキャストして壁があるかチェック
	ACharacterBase* pOwner = Cast<ACharacterBase>(_pOwner);

	// 壁があったらリターン
	if (pOwner->WallCheck(_moveValue)) { return; }

	// 加算
	_pOwner->AddActorWorldOffset(newVector, false);
}



/*
 * 関数名　　　　：GetOwnerActor()
 * 処理内容　　　：オーナーをAActor*で取得
 * 引数１　　　　：USkeletalMeshComponent* MeshComp・・・オーナーのメッシュ
 * 戻り値　　　　：オーナーをAActor*で取得
 */
AActor* UCharacterAnimNotifyState::GetOwnerActor(USkeletalMeshComponent* MeshComp)
{
	// プレイヤーのベースクラス取得
	AActor* pOwner = Cast<AActor>(MeshComp->GetOwner());
	if (!pOwner) { return nullptr; }

	return pOwner;
}