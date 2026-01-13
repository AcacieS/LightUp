// Fill out your copyright notice in the Description page of Project Settings.

#include "LightableActor.h"
#include "Youtube_TutoCharacter.h"  // <-- player class definition
#include "Kismet/GameplayStatics.h" // <-- UGameplayStatics

// Sets default values
ALightableActor::ALightableActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    // Create a scene root
    USceneComponent *SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneRoot;

    // Create light
    LightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightComp"));
    LightComp->SetupAttachment(RootComponent);
    LightComp->SetVisibility(false);

        // LightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("LightComp"));
    // RootComponent = LightComp;
    // LightComp->SetVisibility(false);
}

void ALightableActor::BeginPlay()
{
    Super::BeginPlay();

    AYoutube_TutoCharacter *Player =
        Cast<AYoutube_TutoCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

    if (Player)
    {
        Player->AddMaxLight(MaxLightContribution);
    }
}

void ALightableActor::ActivateLight()
{
    if (LightComp)
    {
        LightComp->SetVisibility(true);
    }
    AYoutube_TutoCharacter *Player =
        Cast<AYoutube_TutoCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

    if (Player)
    {
        Player->AddLight(MaxLightContribution);
    }
}
