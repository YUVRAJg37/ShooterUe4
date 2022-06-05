// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterCharacter.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "In Air"),

	EOS_MAX UMETA(DisplayName = "MAX")
};

UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	public:

	UShooterAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);
	
	virtual void NativeInitializeAnimation() override;

protected:

	void TurnInPlace();

	void Lean(float DeltaTime);

	private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float RotationOffsetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bISAiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bEquipping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsWeaponEquipped{false};

	float TIPCharacterYaw;

	float TIPCharacterYawLastFrame;

	FRotator CharacterRotation;

	FRotator CharacterRotatorLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DeltaYaw;

	float RotationCurve;

	float RotationCurveLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TurnInAnimation, meta = (AllowPrivateAccess = "true"))
	float RootBoneOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TurnInAnimation, meta = (AllowPrivateAccess = "true"))
	float Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TurnInAnimation, meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TurnInAnimation, meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crouch, meta = (AllowPrivateAccess = "true"))
	bool bCrouch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	bool bUseFabrikIk;
	
	bool bTurning;
	
	public:

	FORCEINLINE bool GetIsWeaponEquipped(){return bIsWeaponEquipped;}
	
};
