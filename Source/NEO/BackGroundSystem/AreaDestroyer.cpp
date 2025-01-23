// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaDestroyer.h"
#include "Components/BoxComponent.h"

// Sets default values
AAreaDestroyer::AAreaDestroyer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	// Bind the overlap event
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAreaDestroyer::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AAreaDestroyer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAreaDestroyer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAreaDestroyer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If the overlapped actor has the "Enemy" and "Area1" tags, destroy it
	if (OtherActor && OtherActor->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Area1")))
	{
		OtherActor->Destroy();
	}
}