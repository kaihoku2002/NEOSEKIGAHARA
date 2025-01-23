// Fill out your copyright notice in the Description page of Project Settings.


#include "GimmikGunMan.h"

AGimmikGunMan::AGimmikGunMan()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGimmikGunMan::Tick(float DeltaTime)
{
	bCanMove = false;
}
