// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
// #include "Components/SplineComponent.h"
// #include "Components/SplineMeshComponent.h"
#include "Youtube_TutoCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AYoutube_TutoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ThrowAction;

public:

	/** Constructor */
	AYoutube_TutoCharacter();	
	UPROPERTY(VisibleAnywhere)
		float TestFloat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Firefly;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Light;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float MaxLight = 0.0f;

		// Throw charge
	float ThrowChargeTime = 0.0f;
	bool bIsChargingThrow = false;

	UPROPERTY(EditAnywhere, Category="Throw")
		float MaxChargeTime = 2.0f; // max seconds to charge
	

	// UPROPERTY(VisibleAnywhere, Category="Throw Preview")
	// 	USplineComponent* ThrowPreviewSpline;
	// UPROPERTY(EditAnywhere, Category="Throw Preview")
	// 	UStaticMesh* PreviewSplineMesh;
	// UPROPERTY()
	// 	TArray<USplineMeshComponent*> PreviewMeshes;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	// UPROPERTY(EditAnywhere, Category = "UI HUD")
	// 	TSubclassOf<UUserWidget> Player_Firefly_Widget_Class;
	// 	UUserWidget* Player_Firefly_Widget;
	UPROPERTY(EditAnywhere, Category = "UI HUD")
		TSubclassOf<class ULightUserWidget> PlayerHUDClass;
		ULightUserWidget* PlayerHUD;

	void RestartGame();
	void AddLight(const float light_value);
	void AddMaxLight(const float light_value);

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for throw light input */
	//void Throw(const FInputActionValue& Value);
	
	void StartThrowCharge(const FInputActionValue& Value);
	void ReleaseThrow(const FInputActionValue& Value);
	void UpdateThrowPreview();

public:
	virtual void Tick(float DeltaTime) override;
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AFirefly> FireflyActor;


public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

