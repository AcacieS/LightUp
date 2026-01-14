// Fill out your copyright notice in the Description page of Project Settings.

#include "LightableComponent.h"
#include "Youtube_TutoCharacter.h"
#include "Kismet/GameplayStatics.h"

ULightableComponent::ULightableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULightableComponent::BeginPlay()
{
	Super::BeginPlay();

	// Create a light and attach to owner root
	AActor *Owner = GetOwner();
	if (Owner)
	{
		LightComp = NewObject<UPointLightComponent>(Owner);
		LightComp->RegisterComponent();
		LightComp->AttachToComponent(
			Owner->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform);
		LightComp->SetVisibility(false);
	}

	// Add max light capacity to player
	AYoutube_TutoCharacter *Player =
		Cast<AYoutube_TutoCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (Player)
	{
		Player->AddMaxLight(MaxLightContribution);
	}
}

void ULightableComponent::ActivateLight()
{
	if (LightComp)
	{
		if (LightComp->IsVisible())
			return;
		LightComp->SetVisibility(true);
	}
	if (LightUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			LightUpSound,
			GetOwner()->GetActorLocation());
	}

	AYoutube_TutoCharacter *Player =
		Cast<AYoutube_TutoCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (Player)
	{
		Player->AddLight(MaxLightContribution);
	}
}