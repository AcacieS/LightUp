// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameManager.generated.h"

/**
 * 
 */
UCLASS()
class YOUTUBE_TUTO_API AGameManager : public AGameStateBase
{
	GENERATED_BODY()
public:
    AGameManager();

    UPROPERTY(BlueprintReadOnly)
    float WorldLightPercent = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float WorldLightMax = 100.0f;

    UFUNCTION(BlueprintCallable)
    void AddLight(float Amount);
};
