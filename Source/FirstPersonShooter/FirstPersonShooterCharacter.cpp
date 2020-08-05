// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPersonShooterCharacter.h"
//#include "FirstPersonShooterProjectile.h"
//#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Rifle.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////  (X=32.365479,Y=32.190006,Z=128.441833)
// AFirstPersonShooterCharacter

AFirstPersonShooterCharacter::AFirstPersonShooterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 1.f;
	BaseLookUpRate = 1.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, BaseEyeHeight+70.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bConstrainAspectRatio = true;
	FirstPersonCameraComponent->SetAspectRatio(16/9);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));	
	Mesh1P->SetOnlyOwnerSee(true);	
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);
	
}

void AFirstPersonShooterCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	GetMesh()->SetOwnerNoSee(true);

	Rifle = GetWorld()->SpawnActor<ARifle>(RifleClass);
	Rifle->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	Rifle->GunMesh->SetOnlyOwnerSee(true);
	Rifle->SetOwner(this);
/*
	Rifle3P = GetWorld()->SpawnActor<ARifle>(RifleClass3P);
	Rifle3P->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	Rifle3P->GunMesh->SetOwnerNoSee(true);
	Rifle3P->SetOwner(this);
*/	
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor

}

//////////////////////////////////////////////////////////////////////////  
// Input

void AFirstPersonShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFirstPersonShooterCharacter::OnFire);
	
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFirstPersonShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFirstPersonShooterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AFirstPersonShooterCharacter::TurnLeftRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFirstPersonShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AFirstPersonShooterCharacter::LookUpDown);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFirstPersonShooterCharacter::LookUpAtRate);
}

void AFirstPersonShooterCharacter::OnFire()
{
	// try and fire a projectile
	/*if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();*/
	//	if (World != NULL)
	//	{
	//		if (bUsingMotionControllers)
	//		{
	//			const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
	//			const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
	//			World->SpawnActor<AFirstPersonShooterProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	//		}
	//		else
	//		{
	//			const FRotator SpawnRotation = GetControlRotation();
	//			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	//			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

	//			//Set Spawn Collision Handling Override
	//			FActorSpawnParameters ActorSpawnParams;
	//			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//			// spawn the projectile at the muzzle
	//			World->SpawnActor<AFirstPersonShooterProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	//		}
	//	}
	//}

	//// try and play the sound if specified
	//if (FireSound != NULL)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//}

	//// try and play a firing animation if specified
	//if (FireAnimation != NULL)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	//	if (AnimInstance != NULL)
	//	{
	//		AnimInstance->Montage_Play(FireAnimation, 1.f);
	//	}
	//}
}


void AFirstPersonShooterCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFirstPersonShooterCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFirstPersonShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFirstPersonShooterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFirstPersonShooterCharacter::TurnLeftRight(float Rate)
{
	if (Sensitivity <= 0.f) { Sensitivity = 0.01f; }
	if (Sensitivity > 7.f) { Sensitivity = 7.f; }
	AddControllerYawInput(Rate * Sensitivity);
}

void AFirstPersonShooterCharacter::LookUpDown(float Rate)
{
	if (Sensitivity <= 0.f) { Sensitivity = 0.01f; }
	if (Sensitivity > 7.f) { Sensitivity = 7.f; }
	AddControllerPitchInput(Rate * Sensitivity);
}


