// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightableActor.h"
#include "GameFramework/Actor.h"
#include "WindowActor.generated.h"

UCLASS()
class YOUTUBE_TUTO_API AWindowActor : public ALightableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWindowActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
