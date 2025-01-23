#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TargetPoint.h"
#include "Components/BoxComponent.h"
#include "EnamyBase.h"
#include "Lancer.generated.h"

UCLASS()
class NEO_API ALancer : public AEnamyBase
{
    GENERATED_BODY()

public:
    ALancer();
    UPROPERTY(EditAnywhere, Category = "AI")
        float DesiredDistance = 100.0f; // プレイヤーからの望ましい距離（3m）

    UPROPERTY(EditAnywhere, Category = "AI")
        float MovementSpeed = 100.0f; // 移動速度
    UPROPERTY(EditAnywhere, Category = "AI")
        float SomeMinimumDistance = 100.0f; // 移動速度
    float Speed = 100.0f;
    void ChooseNewTarget();
    void DistanceFromEnemies();
    FTimerHandle EnemyDeath;
    void Death();
    void SppedHighChange();
    void SpeedLowChange();
protected:
    virtual void BeginPlay() override;

    FVector GetSnappedDirection(const FVector& Direction) const;
    
    
    void CollisionOn();

    virtual void Tick(float DeltaTime) override;
    
    

private:
    UPROPERTY(EditAnywhere, Category = "Rotation")
        bool bIsRotation;
    FVector GetPlayerDirection() const;
    float GetDistanceToPlayer() const;
    FTimerHandle TimerHandle_CheckPlayerInFront;
    bool bIsPlayerInFront;
    void CheckPlayerInFront();
    bool bIsRandMove;
    //FVector RoundDirectionT45Degrees(FVector direction) const;
    bool bShouldSkipNextMovement;
    /*float TimeUntilNextRandomMove = 0.0f;
    UPROPERTY(EditAnywhere, Category = "Movement")
        float MinWaitTime = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
        float MaxWaitTime = 5.0f;*/
    FTimerHandle MoveToTargetTimer;
   
    ATargetPoint* CurrentTarget = nullptr;
    
    
    ECollisionResponse DefaultCollisionResponseToPlayers;  // プレイヤーに対するデフォルトのコリジョンレスポンス
    ECollisionResponse DefaultCollisionResponseToWorld;  // 地面（WorldStatic）に対するデフォルトのコリジョンレスポンス
};
