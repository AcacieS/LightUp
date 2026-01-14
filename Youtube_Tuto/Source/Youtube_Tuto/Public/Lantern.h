// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "Lantern.generated.h"

UCLASS()
class YOUTUBE_TUTO_API ALantern : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALantern();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPointLightComponent *LanternLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lantern")
	float LightIncreaseAmount = 100.f;

	UFUNCTION(BlueprintCallable)
	void LightUp();

	UFUNCTION(BlueprintCallable)
	void LightDown();
};
