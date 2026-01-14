// Fill out your copyright notice in the Description page of Project Settings.

#include "Lantern.h"

// Sets default values
ALantern::ALantern()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// LanternLight->Intensity = 0.f;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Light component
	LanternLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LanternLight"));
	LanternLight->SetupAttachment(RootComponent);

	// Default values
	LanternLight->SetIntensity(0.f);
	LanternLight->SetAttenuationRadius(500.f);
	LanternLight->SetVisibility(true);
}

// Called when the game starts or when spawned
void ALantern::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALantern::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ALantern::LightUp()
{
	if (!LanternLight)
		return;

	float Current = LanternLight->Intensity;
	float NewIntensity = Current + LightIncreaseAmount;
	NewIntensity = FMath::Clamp(NewIntensity, 0.f, 10000.f);
	LanternLight->SetIntensity(NewIntensity);
}

void ALantern::LightDown()
{
	if (!LanternLight)
		return;
	float Current = LanternLight->Intensity;
	float NewIntensity = Current - LightIncreaseAmount;
	NewIntensity = FMath::Clamp(NewIntensity, 0.f, 10000.f);
	LanternLight->SetIntensity(NewIntensity);
}
