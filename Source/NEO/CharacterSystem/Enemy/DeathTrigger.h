#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeathTrigger.generated.h"

UCLASS()
class NEO_API ADeathTrigger : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ADeathTrigger();

    // Sphere collision component
    UPROPERTY(VisibleAnywhere, Category = "Trigger")
        class USphereComponent* TriggerSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
        UAnimMontage* MyAnimMontage;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Overlap begin function
    UFUNCTION()
        void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
