// Fill out your copyright notice in the Description page of Project Settings.


#include "RockFlag.h"
#include "NEO/CharacterSystem/PlayerSystem/PlayerBase.h"

// Sets default values
ARockFlag::ARockFlag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FlagBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FlagBox"));
	FlagBox->SetBoxExtent(FVector(10.0f, 10.0f, 10.0f)); // サイズ
	FlagBox->OnComponentBeginOverlap.AddDynamic(this, &ARockFlag::OnOverlap);
	FlagBox->SetupAttachment(RootComponent);
	bIsOnStep = false;
}

// Called when the game starts or when spawned
void ARockFlag::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARockFlag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ARockFlag::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerBase* CastedPlayer = Cast<APlayerBase>(OtherActor);
	if (CastedPlayer)
	{
		if (PusherClass && SpawnPointOne && SpawnPointTwo)
		{
			// 1体目のPusherを出す
			FVector SpawnLocationOne = SpawnPointOne->GetActorLocation();
			FRotator SpawnRotation = FRotator(0.f, 0.f, 0.f);
			AActor* SpawnedPusherOne = GetWorld()->SpawnActor<AActor>(PusherClass, SpawnLocationOne, SpawnRotation);

			// 2体目のPusherを出す
			FVector SpawnLocationTwo = SpawnPointTwo->GetActorLocation();
			AActor* SpawnedPusherTwo = GetWorld()->SpawnActor<AActor>(PusherClass, SpawnLocationTwo, SpawnRotation);



			FlagBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
}
void ARockFlag::SpawnPusher()
{
	
}