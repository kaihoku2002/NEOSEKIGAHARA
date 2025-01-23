// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Destoryer.generated.h"

UCLASS()
class NEO_API ADestoryer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADestoryer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		UStaticMeshComponent* Mesh1;
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* TransparentSkeletalMesh1;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* TransparentSkeletalMesh2;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* TransparentSkeletalMesh3;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* TransparentSkeletalMesh4;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* TransparentSkeletalMesh5;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* TransparentSkeletalMesh6;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		UBoxComponent* ForwardBoxCollider;
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float Damage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	float MoveTime = 0.0f;
	const float MaxMoveTime = 5.0f;
	bool bShouldMoveForward = true;
};
