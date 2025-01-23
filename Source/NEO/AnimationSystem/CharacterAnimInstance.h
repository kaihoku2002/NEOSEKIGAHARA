// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NEO/WeaponSystem/WeaponBase.h"
#include "CharacterAnimInstance.generated.h"

//-----------------武器の種類-------------------------------------
UENUM(BlueprintType)
enum class ECharacterState :uint8
{
	State_Idle   UMETA(DisplayName = "Idle"),
	State_Move   UMETA(DisplayName = "Move"),
	State_Jump   UMETA(DisplayName = "Jump")
};
//----------------------------------------------------------------

UCLASS()
class NEO_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:

	// コンストラクタ
	UCharacterAnimInstance();

	// 開始時処理
	virtual void NativeBeginPlay() override;

	// 毎フレーム処理(Tick) 
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 武器の種類を登録
	void SetWeaponType(EWeaponType _weaponType = EWeaponType::WeaponType_None) { WeaponType = _weaponType; }

	// キャラクターの状態をセット
	void SetCharacterState(ECharacterState _charaState = ECharacterState::State_Idle) { State = _charaState; }

protected:

	// オーナーをキャラクタークラスで取得
	ACharacter* OwnerCharacter;

	// オーナーのMovementComponent保存用
	class UCharacterMovementComponent* CharacterMovementComp;

	// 
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Owner Status")
		FVector Velocity;

	//歩行速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner Status")
		float GroundSpeed;

	// 動いているか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner Status")
		bool ShouldMove;

	// 空中にいる状態かどうか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner Status")
		bool IsFall;

	// 何の武器を持っているか
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner Status")
		EWeaponType WeaponType;

	// キャラクターの状態
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Owner Status")
		ECharacterState State;
};
