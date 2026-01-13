// Fill out your copyright notice in the Description page of Project Settings.

#include "LightUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ULightUserWidget::SetLight(float CurrentLight, float MaxLight)
{
    if (LightBar)
    {
        LightBar->SetPercent(CurrentLight / MaxLight);
        if (CurrentLight >= MaxLight)
        {
            UGameplayStatics::OpenLevel(this, FName("WinMenuLevel"));
        }
    }
}

void ULightUserWidget::SetNbFirefly(int32 CurrentFirefly)
{
    if (FireflyTxt)
    {
        UE_LOG(LogTemp, Warning, TEXT("Inside here should update fireflyTxt"));
        FireflyTxt->SetText(FText::Format(
            FText::FromString("Firefly: {0}"),
            FText::AsNumber(CurrentFirefly)));
    }
}