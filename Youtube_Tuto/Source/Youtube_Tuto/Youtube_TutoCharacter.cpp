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
}
void AYoutube_TutoCharacter::BeginPlay(){
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AYoutube_TutoCharacter::OnBeginOverlap);
	// ThrowPreviewSpline->ClearSplinePoints();
	// if(Player_Firefly_Widget_Class != nullptr) {
	// 	Player_Firefly_Widget = CreateWidget(GetWorld(), Player_Firefly_Widget_Class);
	// 	Player_Firefly_Widget->AddToViewport();

	// }
	//locally controlled when user connected
	// if(IsLocallyControlled() && PlayerHUDClass){
	// 	//own by local player controller
	// 	AYoutube_TutoPlayerController* FPC = GetController<AYoutube_TutoPlayerController>();
	// 	PlayerHUD = CreateWidget<ULightUserWidget>(FPC, PlayerHUDClass);
	// 	PlayerHUD->AddToPlayerScreen();
	// }
	if (IsLocallyControlled() && PlayerHUDClass)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
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
}
//remove from parent, it would clean up?
void AYoutube_TutoCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason){
	if(PlayerHUD){
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
		ThrowChargeTime += DeltaTime;

		// Optional clamp
		if (ThrowChargeTime > MaxChargeTime)
			ThrowChargeTime = MaxChargeTime;
		UpdateThrowPreview();
	}
	// if (bIsChargingThrow)
    // {
    //     ThrowChargeTime += DeltaTime;
    //     ThrowChargeTime = FMath::Min(ThrowChargeTime, MaxChargeTime);

    //     UpdateThrowPreview(); // <-- draw arc
    // }
    // else
    // {
    //     // Clear preview when not charging
    //     for (USplineMeshComponent* PreviewMesh : PreviewMeshes)
    //     {
	// 		if (PreviewMesh)
	// 		{
	// 			PreviewMesh->DestroyComponent();
	// 		}
    //     }
    //     PreviewMeshes.Empty();
    //     ThrowPreviewSpline->ClearSplinePoints();
    // }

}
void AYoutube_TutoCharacter::AddLight(const float light_value){
	Light+=light_value;
	if (PlayerHUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("Add Light"));
		PlayerHUD->SetLight(Light, MaxLight);
	}
}


void AYoutube_TutoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AYoutube_TutoCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AYoutube_TutoCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AYoutube_TutoCharacter::Look);
		
		// Throw light
		//EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AYoutube_TutoCharacter::Throw);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AYoutube_TutoCharacter::StartThrowCharge);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &AYoutube_TutoCharacter::ReleaseThrow);
		
		//triggered, Completed: when you lift of the button it will run this, Started
	}
	else
	{
		UE_LOG(LogYoutube_Tuto, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AYoutube_TutoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AYoutube_TutoCharacter::Look(const FInputActionValue& Value)
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

void AYoutube_TutoCharacter::AddMaxLight(const float light_value){
	MaxLight+=light_value;
	GEngine->AddOnScreenDebugMessage(
		-1, 2.0f, FColor::Green,
		FString::Printf(TEXT("MaxLight: %.0f"), MaxLight)
	);
}
// void AYoutube_TutoCharacter::Throw(const FInputActionValue& Value){
// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Throw Event Pressed"));
// 	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f; 
//     FRotator SpawnRotation = GetActorRotation();

//     GetWorld()->SpawnActor<AFirefly>(FireflyActor, SpawnLocation, SpawnRotation);
// 	//GetWorld()->SpawnActor<AFirefly>(FireflyActor, GetActorLocation(), GetActorRotation());
// }

void AYoutube_TutoCharacter::StartThrowCharge(const FInputActionValue& Value)
{
	bIsChargingThrow = true;
	ThrowChargeTime = 0.0f;
}

void AYoutube_TutoCharacter::ReleaseThrow(const FInputActionValue& Value)
{
	bIsChargingThrow = false;

	// Clamp charge
	float FinalCharge = FMath::Clamp(ThrowChargeTime, 0.0f, MaxChargeTime);

	// Normalize 0 → 1
	float ChargeAlpha = FinalCharge / MaxChargeTime;

	// Debug
	GEngine->AddOnScreenDebugMessage(
		-1, 2.0f, FColor::Green,
		FString::Printf(TEXT("Charge: %.2f"), ChargeAlpha)
	);

	// Spawn firefly
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator SpawnRotation = GetActorRotation();

	AFirefly* FireflyInstance = GetWorld()->SpawnActor<AFirefly>(FireflyActor, SpawnLocation, SpawnRotation);

	if (FireflyInstance)
	{
		// Example: scale launch strength by charge
		float ThrowStrength = FMath::Lerp(500.0f, 2000.0f, ChargeAlpha);
		FireflyInstance->Launch(ThrowStrength); 
	}

	// for (USplineMeshComponent* PreviewMesh : PreviewMeshes)
	// {
	// 	if (PreviewMesh)
	// 		{
	// 			PreviewMesh->DestroyComponent();
	// 		}
	// }
	// PreviewMeshes.Empty();
	// ThrowPreviewSpline->ClearSplinePoints();
}

void AYoutube_TutoCharacter::RestartGame(){
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	//then there is like on player BP Mesh, Collision: Collision Preset: Pawn->Ragdoll
	//New FOlder(physics)-> Physics:Physics asset: skeleton mesh. 
	//Physics, additional information: currently used Physics Asset: name of physics asset wanted
	
}
void AYoutube_TutoCharacter::OnBeginOverlap(class UPrimitiveComponent* HitComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult &SweepResult){
		if(OtherActor->ActorHasTag("Candy")){
			//UE_LOG(LogTemp, Warning, TEXT("Collided with"));
			Firefly += 1.0f;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Firefly: %.1f"), Firefly));
			if (PlayerHUD)
			{
				UE_LOG(LogTemp, Warning, TEXT("Has Updated"));
				PlayerHUD->SetNbFirefly((int32)Firefly);
			}
			// if(Firefly > 100.0f)
			// 	Firefly = 100.0f;
			
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Firefly: %.1f"), Firefly));
			OtherActor->Destroy();
		}

}

// void AYoutube_TutoCharacter::UpdateThrowPreview()
// {
//     if (!PreviewSplineMesh) return;

//     // Clear old meshes
//     for (USplineMeshComponent* PreviewMesh : PreviewMeshes)
//     {
//         if (PreviewMesh)
// 		{
// 			PreviewMesh->DestroyComponent();
// 		}
//     }
//     PreviewMeshes.Empty();
//     ThrowPreviewSpline->ClearSplinePoints();

//     // === Compute charge ===
//     float ChargeAlpha = ThrowChargeTime / MaxChargeTime;
//     float ThrowStrength = FMath::Lerp(300.f, 1500.f, ChargeAlpha);
//     float ForwardSpeed = 1000.f;

//     FVector StartLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
//     FVector LaunchVelocity =
//         (GetActorForwardVector() * ForwardSpeed) +
//         (FVector::UpVector * ThrowStrength);

//     // === Predict Path ===
//     FPredictProjectilePathParams Params;
//     Params.StartLocation = StartLocation;
//     Params.LaunchVelocity = LaunchVelocity;
//     Params.bTraceWithCollision = true;
//     Params.ProjectileRadius = 5.f;
//     Params.MaxSimTime = 3.f;
//     Params.TraceChannel = ECC_Visibility;

//     FPredictProjectilePathResult Result;
//     bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);

//     // === Build spline ===
//     for (FPredictProjectilePathPointData& Point : Result.PathData)
//     {
//         ThrowPreviewSpline->AddSplinePoint(Point.Location, ESplineCoordinateSpace::World);
//     }

//     ThrowPreviewSpline->UpdateSpline();

//     // === Create spline mesh segments ===
//     int32 NumPoints = ThrowPreviewSpline->GetNumberOfSplinePoints();
//     for (int32 i = 0; i < NumPoints - 1; i++)
//     {
//         USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
//         SplineMesh->SetStaticMesh(PreviewSplineMesh);
// 		SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//         SplineMesh->RegisterComponent();
//         SplineMesh->SetMobility(EComponentMobility::Movable);
//         SplineMesh->AttachToComponent(ThrowPreviewSpline, FAttachmentTransformRules::KeepWorldTransform);

//         FVector StartPos, StartTangent, EndPos, EndTangent;
//         ThrowPreviewSpline->GetLocationAndTangentAtSplinePoint(i, StartPos, StartTangent, ESplineCoordinateSpace::World);
//         ThrowPreviewSpline->GetLocationAndTangentAtSplinePoint(i+1, EndPos, EndTangent, ESplineCoordinateSpace::World);

//         SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);

//         PreviewMeshes.Add(SplineMesh);
//     }
// }

void AYoutube_TutoCharacter::UpdateThrowPreview()
{
    float ChargeAlpha = ThrowChargeTime / MaxChargeTime;
    float ThrowStrength = FMath::Lerp(300.f, 1500.f, ChargeAlpha);
    float ForwardSpeed = 1000.f;

    FVector StartLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
    FVector LaunchVelocity =
        (GetActorForwardVector() * ForwardSpeed) +
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

    // Draw debug line
    for (int32 i = 0; i < Result.PathData.Num() - 1; i++)
    {
        DrawDebugLine(
            GetWorld(),
            Result.PathData[i].Location,
            Result.PathData[i+1].Location,
            FColor::Yellow,
            false,   // not persistent
            0.0f,    // single frame
            0,
            2.0f
        );
    }
}
