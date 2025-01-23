//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "BossAnimInstance.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "BossBase.h"
//
//void UBossAnimInstance::NativeInitializeAnimation()
//{
//	if (MyBossChar == nullptr)
//	{
//		MyBossChar = Cast<ABossBase>(TryGetPawnOwner());
//	}
//}
//
//void UBossAnimInstance::UpdateAnimationProperties(float DeltaTime)
//{
//	if (MyBossChar == nullptr)
//	{
//		MyBossChar = Cast<ABossBase>(TryGetPawnOwner());
//	}
//	if (MyBossChar)
//	{
//		FVector Velocity = MyBossChar->GetVelocity();
//		Velocity.Z = 0;
//		Speed = Velocity.Size();
//		bIsInAir = MyBossChar->GetMovementComponent()->IsFalling();
//		if (MyBossChar->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
//		{
//			bIsAccelerating = true;
//		}
//		else
//		{
//			bIsAccelerating = false;
//		}
//	}
//}