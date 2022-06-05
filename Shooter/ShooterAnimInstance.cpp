// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
Speed(0),
bIsInAir(false),
bIsAccelerating(false),
bISAiming(false),
RotationOffsetYaw(0),
LastMovementOffset(0),
TIPCharacterYaw(0),
TIPCharacterYawLastFrame(0),
RootBoneOffset(0),
Pitch(0),
OffsetState(EOffsetState::EOS_Hip),
DeltaYaw(0),
CharacterRotation(0),
CharacterRotatorLastFrame(0),
bTurning(false),
RecoilWeight(1.0f),
bEquipping(false),
EquippedWeaponType(EWeaponType::EWT_MAX),
bUseFabrikIk(false)
{
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		bCrouch = ShooterCharacter->GetCrouching();
		
		bIsReloading = ShooterCharacter->GetCombateState() == ECombateState::ECS_Reloading;
		bEquipping = ShooterCharacter->GetCombateState() == ECombateState::ECS_Equipping;
		bUseFabrikIk = ShooterCharacter->GetCombateState() == ECombateState::ECS_Unoccupied || ShooterCharacter->GetCombateState() == ECombateState::ECS_FireTimerInProgress;
		
		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		RotationOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		FString OffsetMessage = FString::Printf(TEXT("Offset : %f"), RotationOffsetYaw);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0, FColor::White, OffsetMessage);
		}

		if (ShooterCharacter->GetVelocity().Size() > 0)
		{
			LastMovementOffset = RotationOffsetYaw;
		}

		bISAiming = ShooterCharacter->GetIsAiming();

		if (bIsReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetIsAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}

		if(ShooterCharacter->GetEquippedWeapon())
		{
			EquippedWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
		}
	}

	TurnInPlace();
	Lean(DeltaTime);
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr)
	{
		return;
	}

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		RootBoneOffset = 0;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurve = 0;
		RotationCurveLastFrame = 0;
		Pitch = 0;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		float DeltaOffset = TIPCharacterYaw - TIPCharacterYawLastFrame;

		RootBoneOffset = UKismetMathLibrary::NormalizeAxis(RootBoneOffset - DeltaOffset);

		const float Turning = GetCurveValue(TEXT("Turning"));

		if (Turning > 0)
		{
			bTurning = true;
			
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));

			float DeltaRotation = RotationCurve - RotationCurveLastFrame;

			RootBoneOffset > 0 ? RootBoneOffset -= DeltaRotation : RootBoneOffset += DeltaRotation;

			float ABSRootBoneOffset = FMath::Abs(RootBoneOffset);

			if (ABSRootBoneOffset > 90)
			{
				const float ExcessYaw = ABSRootBoneOffset - 90;

				RootBoneOffset > 0 ? RootBoneOffset -= ExcessYaw : RootBoneOffset += ExcessYaw;
			}
		}
		else
		{
			bTurning = false;
		}
	}
	
	if(bTurning)
	{
		if(bIsReloading || bEquipping)
		{
			RecoilWeight = 1.0f;
		}
		else
		{
			RecoilWeight = 0.5f;
		}
	}
	else
	{
		if(bCrouch)
		{
			if(bIsReloading || bEquipping)
			{
				RecoilWeight = 1.0f;
			}
			else
			{
				RecoilWeight = 0.25f;
			}
		}
		else
		{
			RecoilWeight = 1.0f;
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		return;
	}

	CharacterRotatorLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();

	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotatorLastFrame);

	const float Target = (DeltaRotation.Yaw) / DeltaTime;
	float Interp{FMath::FInterpTo(DeltaYaw, Target, DeltaTime, 6.0f)};

	DeltaYaw = FMath::Clamp(Interp, -90.f, 90.f);
	
}
