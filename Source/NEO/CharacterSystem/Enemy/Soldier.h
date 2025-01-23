#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnamyBase.h"
#include "Soldier.generated.h"

UCLASS()
class NEO_API ASoldier : public AEnamyBase
{
    GENERATED_BODY()

public:
    ASoldier();
    UPROPERTY(EditAnywhere, Category = "AI")
        float DesiredDistance = 100.0f; // プレイヤーからの望ましい距離（3m）

   
    float MoveSpeed = 200.0f;
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    UFUNCTION(Blueprintcallable)
        void AttackCombo();
    FVector GetSnappedDirection(const FVector& Direction) const;

    void CollisionOn();
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    int32 ComboCounter;
    FTimerHandle ComboResetTimer;
    void ResetCombo();
    FVector GetPlayerDirection() const;
    float GetDistanceToPlayer() const;
    float AttackCooldown;
    float LastAttackTime;
    bool bIsAttacking;
    bool bIsOnCooldown;
    FTimerHandle ComboCooldownTimer;
    void EndComboCooldown();
};

    