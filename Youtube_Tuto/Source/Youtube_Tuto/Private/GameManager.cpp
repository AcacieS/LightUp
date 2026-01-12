// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"

AGameManager::AGameManager(){
    WorldLightPercent = 0.f;
}


void AGameManager::AddLight(float Amount){
    WorldLightPercent = WorldLightPercent + Amount;
}