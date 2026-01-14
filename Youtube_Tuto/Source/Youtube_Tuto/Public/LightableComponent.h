// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PointLightComponent.h"
#include "Sound/SoundBase.h"
#include "LightableComponent.generated.h"

class AYoutube_TutoCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class YOUTUBE_TUTO_API ULightableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULightableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Light")
	float MaxLightContribution = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPointLightComponent *LightComp;

	UPROPERTY(EditAnywhere, Category = "Light|Sound")
	USoundBase *LightUpSound;

public:
	UFUNCTION(BlueprintCallable)
	void ActivateLight();
};
