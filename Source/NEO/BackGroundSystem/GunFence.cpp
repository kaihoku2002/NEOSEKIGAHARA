// Fill out your copyright notice in the Description page of Project Settings.


#include "GunFence.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "NEO/CharacterSystem/Enemy/GunMan.h"
AGunFence::AGunFence()
{
    MaxHealth = 100;
    Health = MaxHealth;
}

ATargetPoint* AGunFence::FindNearestAvailableTargetPoint(const AActor* Actor)
{
    ATargetPoint* NearestTargetPoint = nullptr;
    float NearestDistance = FLT_MAX;

    for (ATargetPoint* TargetPoint : TargetPoints)
    {
        float Distance = FVector::Dist(TargetPoint->GetActorLocation(), Actor->GetActorLocation());

        if (Distance < NearestDistance)
        {
            bool bIsOccupied = false;

            // Check if there is a GunMan near the target point
            for (TActorIterator<AGunMan> It(GetWorld()); It; ++It)
            {
                if (FVector::Dist(It->GetActorLocation(), TargetPoint->GetActorLocation()) < 200.f)
                {
                    bIsOccupied = true;
                    break;
                }
            }

            if (!bIsOccupied)
            {
                NearestTargetPoint = TargetPoint;
                NearestDistance = Distance;
            }
        }
    }

    return NearestTargetPoint;
}
