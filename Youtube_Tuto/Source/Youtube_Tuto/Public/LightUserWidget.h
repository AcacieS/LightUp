// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LightUserWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class YOUTUBE_TUTO_API ULightUserWidget : public UUserWidget
{
	GENERATED_BODY()
	public:
		void SetLight(float CurrentLight, float MaxLight);

		void SetNbFirefly(int32 CurrentFirefly);

		UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* LightBar;

		UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UTextBlock* FireflyTxt; //need to be txt

};
