//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Animation/AnimInstance.h"
//
//#include "BossAnimInstance.generated.h"
//
///**
// * 
// */
//UCLASS()
//class NEO_API UBossAnimInstance : public UCharacterAnimInstance
//{
//	GENERATED_BODY()
//	
//public:
//	virtual void NativeInitializeAnimation() override;
//
//	UFUNCTION(BlueprintCallable, Category = Animation)
//	void UpdateAnimationProperties(float DeltaTime);
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
//	float Speed;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
//	bool bIsAccelerating;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
//	bool bIsInAir;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
//	class ABossBase* MyBossChar;
//};
