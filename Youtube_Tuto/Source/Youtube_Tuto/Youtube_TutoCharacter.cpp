// Copyright Epic Games, Inc. All Rights Reserved.

#include "Youtube_TutoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "LightUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Firefly.h"
#include "Lantern.h"
#include "Youtube_Tuto.h"

AYoutube_TutoCharacter::AYoutube_TutoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	PlayerHUDClass = nullptr;
	PlayerHUD = nullptr;

	// ThrowPreviewSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ThrowPreviewSpline"));
	// ThrowPreviewSpline->SetupAttachment(RootComponent);
	TrajectoryMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("TrajectoryMesh"));
	TrajectoryMesh->SetupAttachment(RootComponent);
	TrajectoryMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TrajectoryMesh->SetMobility(EComponentMobility::Movable);

	TrajectoryMesh->SetAbsolute(true, true, true);

	TargetDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("TargetDecal"));
	TargetDecal->SetupAttachment(RootComponent);
	TargetDecal->SetVisibility(false);
	TargetDecal->DecalSize = FVector(64, 64, 64);

	// Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	// Sprite->SetupAttachment(RootComponent);

	// AnimComponent = CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("AnimComponent"));
	// AnimComponent->SetRenderComponent(Sprite);
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(RootComponent);

	AnimComponent = CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("AnimComponent"));
}

void AYoutube_TutoCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AYoutube_TutoCharacter::OnBeginOverlap);

	if (TrajectorySegmentMesh)
	{
		TrajectoryMesh->SetStaticMesh(TrajectorySegmentMesh);
	}
	TargetDecal->SetVisibility(false);

	if (IsLocallyControlled() && PlayerHUDClass)
	{
		APlayerController *PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PlayerHUD = CreateWidget<ULightUserWidget>(PC, PlayerHUDClass);
			UE_LOG(LogTemp, Warning, TEXT("HUD instance: %s"), *PlayerHUD->GetName());
			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
			}
		}
	}
	if (LanternClass)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(50, 0, 50);
		FRotator SpawnRotation = FRotator::ZeroRotator;

		LanternInstance = GetWorld()->SpawnActor<ALantern>(
			LanternClass,
			SpawnLocation,
			SpawnRotation);

		if (LanternInstance)
		{
			LanternInstance->AttachToComponent(
				RootComponent,
				FAttachmentTransformRules::KeepRelativeTransform);

			// Position it relative to your character
			LanternInstance->SetActorRelativeLocation(FVector(40.f, 0.f, 60.f));
			LanternInstance->SetActorRelativeRotation(FRotator::ZeroRotator);
			LanternInstance->SetActorRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		}
	}
}
// remove from parent, it would clean up?
void AYoutube_TutoCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
		PlayerHUD = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AYoutube_TutoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsChargingThrow)
	{
		ThrowChargeTime += DeltaTime * throwMvtTime;

		// Optional clamp
		if (ThrowChargeTime > MaxChargeTime)
			ThrowChargeTime = MaxChargeTime;
		UpdateThrowPreview();
	}
}
void AYoutube_TutoCharacter::AddLight(const float light_value)
{
	Light += light_value;
	if (PlayerHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("Add Light"));
		PlayerHUD->SetLight(Light, MaxLight);
	}
}

void AYoutube_TutoCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AYoutube_TutoCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AYoutube_TutoCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AYoutube_TutoCharacter::Look);

		// Throw light
		// EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AYoutube_TutoCharacter::Throw);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AYoutube_TutoCharacter::StartThrowCharge);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &AYoutube_TutoCharacter::ReleaseThrow);

		// triggered, Completed: when you lift of the button it will run this, Started
	}
	else
	{
		UE_LOG(LogYoutube_Tuto, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AYoutube_TutoCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AYoutube_TutoCharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AYoutube_TutoCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AYoutube_TutoCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AYoutube_TutoCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AYoutube_TutoCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AYoutube_TutoCharacter::AddMaxLight(const float light_value)
{
	MaxLight += light_value;
	GEngine->AddOnScreenDebugMessage(
		-1, 2.0f, FColor::Green,
		FString::Printf(TEXT("MaxLight: %.0f"), MaxLight));
}

void AYoutube_TutoCharacter::StartThrowCharge(const FInputActionValue &Value)
{
	if (Firefly <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("No Firefly"), Firefly));
		return;
	}
	// UPaperZDAnimationComponent *AnimComp = Cast<UPaperZDAnimationComponent>(Actor->GetComponentByClass(UPaperZDAnimationComponent::StaticClass()));
	// if (AnimComp)
	// {
	// 	UPaperZDAnimInstance *AnimInst = AnimComp->GetAnimInstance();
	// 	// Access variables here
	// }
	// if (AnimComponent)
	// {
	// 	AnimComponent->SetBoolParameter(TEXT("bIsChargingThrow"), true);
	// }
	bIsChargingThrow = true;
	Firefly -= 1.0f;
	if (LanternInstance)
	{
		LanternInstance->LightDown();
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Firefly: %.1f"), Firefly));
	if (PlayerHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("Has Updated"));
		PlayerHUD->SetNbFirefly((int32)Firefly);
	}

	ThrowChargeTime = 0.0f;
}

void AYoutube_TutoCharacter::ReleaseThrow(const FInputActionValue &Value)
{
	if (!bIsChargingThrow)
		return;

	bIsChargingThrow = false;
	// if (AnimComponent)
	// {
	// 	AnimComponent->SetBoolParameter(TEXT("bIsChargingThrow"), false);
	// }
	TargetDecal->SetVisibility(false);
	TrajectoryMesh->ClearInstances();

	// Clamp charge
	float FinalCharge = FMath::Clamp(ThrowChargeTime, 0.0f, MaxChargeTime);

	// Normalize 0 → 1
	float ChargeAlpha = FinalCharge / MaxChargeTime;

	// Debug
	GEngine->AddOnScreenDebugMessage(
		-1, 2.0f, FColor::Green,
		FString::Printf(TEXT("Charge: %.2f"), ChargeAlpha));

	// Spawn firefly
	// FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	// FRotator SpawnRotation = GetActorRotation();
	FRotator ControlRot = GetControlRotation();
	FRotator YawOnly(0.f, ControlRot.Yaw, 0.f);

	FVector ForwardDir = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::X);

	FVector SpawnLocation = GetActorLocation() + ForwardDir * 100.f;
	FRotator SpawnRotation = YawOnly;

	AFirefly *FireflyInstance = GetWorld()->SpawnActor<AFirefly>(FireflyActor, SpawnLocation, SpawnRotation);

	if (FireflyInstance)
	{
		// Example: scale launch strength by charge
		float ThrowStrength = FMath::Lerp(500.0f, 2000.0f, ChargeAlpha);
		FireflyInstance->Launch(ThrowStrength);
	}
}

void AYoutube_TutoCharacter::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	// then there is like on player BP Mesh, Collision: Collision Preset: Pawn->Ragdoll
	// New FOlder(physics)-> Physics:Physics asset: skeleton mesh.
	// Physics, additional information: currently used Physics Asset: name of physics asset wanted
}
void AYoutube_TutoCharacter::OnBeginOverlap(class UPrimitiveComponent *HitComp,
											class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
											bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor->ActorHasTag("Candy"))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Collided with"));
		Firefly += 1.0f;
		if (LanternInstance)
		{
			LanternInstance->LightUp();
		}
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Firefly: %.1f"), Firefly));
		if (PlayerHUD)
		{
			UE_LOG(LogTemp, Warning, TEXT("Has Updated"));
			PlayerHUD->SetNbFirefly((int32)Firefly);
		}
		// if(Firefly > 100.0f)
		// 	Firefly = 100.0f;

		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Firefly: %.1f"), Firefly));
		OtherActor->Destroy();
	}
}
void AYoutube_TutoCharacter::UpdateThrowPreview()
{
	float ChargeAlpha = ThrowChargeTime / MaxChargeTime;
	// float ThrowStrength = FMath::Lerp(300.f, 1500.f, ChargeAlpha);
	float ThrowStrength = FMath::Lerp(500.0f, 2000.0f, ChargeAlpha);

	float ForwardSpeed = 700.f;

	FRotator ControlRot = GetControlRotation();
	FRotator YawOnly(0.f, ControlRot.Yaw, 0.f);

	FVector ForwardDir = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::X);

	FVector StartLocation = GetActorLocation() + ForwardDir * 100.f;

	FVector LaunchVelocity =
		(ForwardDir * ForwardSpeed) +
		(FVector::UpVector * ThrowStrength);

	FPredictProjectilePathParams Params;
	Params.StartLocation = StartLocation;
	Params.LaunchVelocity = LaunchVelocity;
	Params.bTraceWithCollision = true;
	Params.ProjectileRadius = 5.f;
	Params.MaxSimTime = 3.f;
	Params.TraceChannel = ECC_Visibility;

	FPredictProjectilePathResult Result;
	UGameplayStatics::PredictProjectilePath(this, Params, Result);

	TrajectoryMesh->ClearInstances();
	// Draw debug line
	for (int32 i = 0; i < Result.PathData.Num() - 1; i += 8)
	{
		for (const FPredictProjectilePathPointData &Point : Result.PathData)
		{
			FTransform T(FRotator::ZeroRotator, Point.Location, FVector(0.02f));
			TrajectoryMesh->AddInstance(T, true);
		}
	}
	if (Result.PathData.Num() > 0)
	{
		// FVector EndLocation = Result.PathData.Last().Location;
		FVector EndLocation = Result.HitResult.bBlockingHit
								  ? Result.HitResult.Location
								  : Result.PathData.Last().Location;
		TargetDecal->SetWorldLocation(EndLocation + FVector(0, 0, 0));
		// TargetDecal->SetWorldLocation(EndLocation);
		// TargetDecal->SetWorldRotation(FRotator(-90, 0, 0)); // project onto ground
		if (Result.HitResult.bBlockingHit)
		{
			FVector Normal = Result.HitResult.ImpactNormal;

			// Build rotation so decal faces the surface
			FRotator DecalRotation = Normal.Rotation();

			// Decals project along their -Z axis, so rotate 180 degrees
			DecalRotation.Pitch += 180.f;

			TargetDecal->SetWorldRotation(DecalRotation);
		}
		TargetDecal->SetVisibility(true);
	}
	else
	{
		TargetDecal->SetVisibility(false);
	}
}
