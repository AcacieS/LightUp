// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightableActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Firefly.generated.h"

UCLASS()
class YOUTUBE_TUTO_API AFirefly : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFirefly();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//class make that you don't need include
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* FireflyMesh;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* FireflyComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Throw")
	float ForwardSpeed = 1000.0f;

	UFUNCTION()
	void OnFireflyOverlap(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);
	
	UFUNCTION()
    void OnFireflyHit(UPrimitiveComponent* HitComp,
                    AActor* OtherActor,
                    UPrimitiveComponent* OtherComp,
                    FVector NormalImpulse,
                    const FHitResult& Hit);

	void Launch(float ThrowStrength);

};
