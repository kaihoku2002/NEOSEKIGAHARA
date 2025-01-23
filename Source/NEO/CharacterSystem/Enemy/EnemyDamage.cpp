// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDamage.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "NEO/CharacterSystem/PlayerSystem/PlayerBase.h"

// Sets default values
AEnemyDamage::AEnemyDamage()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent = BoxComponent;

	// Set the sphere's collision profile
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));

	// Set the size of the sphere
	BoxComponent->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	if (BoxComponent)
	{
		// Set the function to be called when a collision begins
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyDamage::OnOverlapBegin);
	}
	// Set the default damage amount
	DamageAmount = 5.0f;
}

// Called when the game starts or when spawned
void AEnemyDamage::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Lancer"), FoundActors);
		for (AActor* Actor : FoundActors)
		{
			ALancer* Lancer = Cast<ALancer>(Actor);
			if (Lancer)
			{
				Lancers.Add(Lancer);
			}
		}
	}
}

// Called every frame
void AEnemyDamage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyDamage::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsLancerAttacking() && OtherActor && OtherComp && OtherActor != this)
	{
		APlayerBase* PlayerCharacter = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (PlayerCharacter && OtherActor == PlayerCharacter)
		{
			// Apply damage to the player
			PlayerCharacter->TakedDamage(DamageAmount);

			if (OverlappedComp)
			{
				OverlappedComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				FTimerHandle UnusedHandle;
				if (GetWorld())
				{
					GetWorldTimerManager().SetTimer(UnusedHandle, this, &AEnemyDamage::EnableCollision, 3.0f, false);
				}
			}
		}
	}
}
void AEnemyDamage::EnableCollision()
{

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
FCriticalSection MyMutex;
bool AEnemyDamage::IsLancerAttacking()
{

	UAnimMontage* Attack = LoadObject<UAnimMontage>(NULL, TEXT("/Game/0102/Enemy/Lancer/Attacking.Attacking"));

	if (Attack)
	{
		for (ALancer* Lancer : Lancers)
		{
			UAnimInstance* AnimInstance = nullptr;

			if (Lancer && Lancer->GetMesh())
			{
				AnimInstance = Lancer->GetMesh()->GetAnimInstance();
			}
			if (AnimInstance != nullptr)
			{
				// Mutexをロック
				MyMutex.Lock();

				bool isPlaying = AnimInstance->Montage_IsPlaying(Attack);

				// Mutexをアンロック
				MyMutex.Unlock();

				if (isPlaying)
				{
					return true;
				}
			}
		}
	}

	return false;
	/*
	if (!Attack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack is not loaded correctly."));
		return false;
	}

	for (ALancer* Lancer : Lancers)
	{
		if (!Lancer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Lancer is nullptr."));
			continue;
		}

		USkeletalMeshComponent* Mesh = Lancer->GetMesh();
		if (!Mesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Lancer's mesh is nullptr."));
			continue;
		}

		UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
		if (!AnimInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("AnimInstance is nullptr."));
			continue;
		}

		// Mutexをロック
		MyMutex.Lock();

		bool isPlaying = AnimInstance->Montage_IsPlaying(Attack);

		// Mutexをアンロック
		MyMutex.Unlock();

		if (!isPlaying)
		{
			UE_LOG(LogTemp, Warning, TEXT("Montage is not playing."));
		}
		else
		{
			return true;
		}
	}

	return false;
	*/
}