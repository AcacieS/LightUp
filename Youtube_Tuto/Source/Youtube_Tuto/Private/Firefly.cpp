// Fill out your copyright notice in the Description page of Project Settings.

#include "Firefly.h"
#include "LightableComponent.h"

// Sets default values
AFirefly::AFirefly()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; // no need bullet taking every frame
	FireflyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Firefly Mesh"));
	RootComponent = FireflyMesh;

	FireflyComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Firefly Component"));
	FireflyComponent->InitialSpeed = 800;
	FireflyComponent->MaxSpeed = 8500;
	// FireflyComponent->ProjectileGravityScale = 0.3f;

	FireflyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FireflyMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	FireflyMesh->SetGenerateOverlapEvents(true);
	FireflyMesh->SetNotifyRigidBodyCollision(true); // <-- enables Hit events
}

// Called when the game starts or when spawned
void AFirefly::BeginPlay()
{
	Super::BeginPlay();
	FireflyMesh->OnComponentBeginOverlap.AddDynamic(this, &AFirefly::OnFireflyOverlap);
	FireflyMesh->OnComponentHit.AddDynamic(this, &AFirefly::OnFireflyHit);
}

// Called every frame
void AFirefly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AFirefly::OnFireflyOverlap(UPrimitiveComponent *OverlappedComp,
								AActor *OtherActor,
								UPrimitiveComponent *OtherComp,
								int32 OtherBodyIndex,
								bool bFromSweep,
								const FHitResult &SweepResult)
{
	if (!OtherActor || OtherActor == this)
		return;
	ULightableComponent *LightComp =
		OtherActor->FindComponentByClass<ULightableComponent>();

	if (LightComp)
	{
		// GEngine->AddOnScreenDebugMessage(
		// 	-1, 2.f, FColor::Cyan, TEXT("Firefly hit Lightable object!"));

		LightComp->ActivateLight();
		Destroy(); // destroy firefly
	}
	// }else if (OtherActor)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("Destroyed"));
	// 	// treat as obstacle
	// 	Destroy();
	// }
	// if (OtherActor->ActorHasTag("Window"))
	// {
	//     GEngine->AddOnScreenDebugMessage(
	//         -1, 2.f, FColor::Cyan, TEXT("Firefly hit Window!"));

	//     Destroy(); // destroy the firefly itself
	//}
}
void AFirefly::OnFireflyHit(UPrimitiveComponent *HitComp,
							AActor *OtherActor,
							UPrimitiveComponent *OtherComp,
							FVector NormalImpulse,
							const FHitResult &Hit)
{
	ULightableComponent *LightComp =
		OtherActor->FindComponentByClass<ULightableComponent>();

	if (LightComp)
	{
		// GEngine->AddOnScreenDebugMessage(
		// 	-1, 2.f, FColor::Cyan, TEXT("Firefly hit Lightable object!"));

		LightComp->ActivateLight();
	}

	Destroy();
}
void AFirefly::Launch(float ThrowStrength)
{
	if (!FireflyComponent)
		return;

	FVector ForwardDir = GetActorForwardVector();

	// Up direction
	FVector UpDir = FVector::UpVector;

	// Final velocity = forward + upward
	FVector FinalVelocity = (ForwardDir * ForwardSpeed) + (UpDir * ThrowStrength);

	FireflyComponent->Velocity = FinalVelocity;
}
float AFirefly::GetFireflySpeed()
{
	return ForwardSpeed;
}
