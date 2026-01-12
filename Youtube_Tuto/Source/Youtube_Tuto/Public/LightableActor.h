// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "LightableActor.generated.h"

class AYoutube_TutoCharacter; // <-- ADD THIS

UCLASS(Abstract)
class YOUTUBE_TUTO_API ALightableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightableActor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPointLightComponent* LightComp;

	UPROPERTY(EditAnywhere, Category="Light")
	float MaxLightContribution = 1.f;

	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	virtual void ActivateLight();

};
