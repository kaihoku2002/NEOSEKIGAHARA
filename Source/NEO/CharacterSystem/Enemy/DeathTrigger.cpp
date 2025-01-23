#include "DeathTrigger.h"
#include "Components/SphereComponent.h"

#include "GameFramework/Character.h"

// Sets default values
ADeathTrigger::ADeathTrigger()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Create and position a sphere component
    TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
    RootComponent = TriggerSphere;

    // Set the sphere's collision profile
    TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));

    // Event is triggered when sphere begins to overlap another actor
    TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ADeathTrigger::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ADeathTrigger::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ADeathTrigger::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADeathTrigger::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Enemy"))
    {
        // Assuming the other actor is a character
        ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
        if (OtherCharacter && OtherCharacter->GetMesh() && MyAnimMontage)
        {
            OtherCharacter->PlayAnimMontage(MyAnimMontage);
        }
    }
}
