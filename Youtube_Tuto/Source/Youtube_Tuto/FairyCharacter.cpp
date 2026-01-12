// Fill out your copyright notice in the Description page of Project Settings.


#include "FairyCharacter.h"

// Sets default values
AFairyCharacter::AFairyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	//pointer ->
	//not allowed character to rotate
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Allow character rotate in direction it is moving
	//rate rotate
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Camera spring arm component, Name will be CameraBoom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); //attach to be child of root
	CameraBoom->TargetArmLength = 300.0f; //how far camera from player
	CameraBoom->bUsePawnControlRotation = true; //rotate arm based on controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//attach camera to camera boom: let boom adjust the match controller of the camera
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; //Does not rotate relative to the orb??

	bDead = false;

}

// Called when the game starts or when spawned
void AFairyCharacter::BeginPlay()
{
	Super::BeginPlay();
}


// Called to bind functionality to input
void AFairyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFairyCharacter::MoveForward(float Axis){

}
void AFairyCharacter::MoveRight(float Axis){
	
}


