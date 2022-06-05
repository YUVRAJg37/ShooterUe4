// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ammo.h"
#include "Item.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "AmmoType.h"
#include "WeaponType.h"
#include "ShooterCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombateState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),

	ECS_MAX UMETA(DisplayName = "Default")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NextSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UENUM(BlueprintType)
enum class EWeaponMode : uint8
{
	EWM_Single UMETA(DisplayName = "Single"),
	EWM_Auto UMETA(DisplayName = "Auto"),
	EWM_Burst UMETA(DisplayName = "Burst"),

	ECS_MAX UMETA(DisplayName = "Default")
};


UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveLeft(float value);
	void AimInterp(float DeltaTime);
	void ScopePressed();
	void ScopeReleased();
	void GetScopeSensi();
	void MouseLook(float Value);
	void MouseTurn(float Value);

	void CalculateCrossHairSpread(float DeltaTime);
	void StartCrossHairFire();
	void FinishCrossHairFire();
	void FireButtonPressed();
	void FireButtonStopped();
	void AutoFireTimer();
	void AutoFire();
	void SwitchingMode();
	bool TraceUnderCrosshair(FHitResult& OutHitResult, FVector& HitLocation);
	void ItemTrace();

	AWeapon* SpawnDefaultWeapon();

	void EquipWeapon(AWeapon* Weapon, bool bSwapping = false);

	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	void Swap(AWeapon* Weapon);
	void InitializeAmmoMap();
	void PlayFireSound();
	void SendBullet();
	void PlayMontage();
	void SingleFire();
	void Burst();
	void EndBurst();

	void ReloadButtonPressed();
	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReload();
	UFUNCTION(BlueprintCallable)
	void GrabClip();
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	
	void Crouch();
	void Jump();
	void InterpCapsuleHeight(float DeltaTime);
	void Aiming();
	void StopAiming();
	void PickUpAmmo(AAmmo* Ammo);

	int32 GetEmptyInventorySlots();

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	FTimerHandle PickupSoundHandler;
	FTimerHandle EquipSoundHandler;

	void ResetEquipSound();
	void ResetPickupSound();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float PickupSoundDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float EquipSoundDelay;

	UFUNCTION(BlueprintCallable)
	void EndStun();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void FireWeapon();
	
	void GetPickUpItem(AItem* Item);

	bool bIsAmmoLeft();

	bool CarryingAmmo();

	void InitializeInterpLocations();
	FInterpLocation GetInterpLocation(int32 Index);
	int32 GetInterpLocationIndex();
	void IncrementItemCount(int32 Index, int32 Amount);

	void StartPickupSound();
	void StartEquipSound();

	void FKey();
	void OneKey();
	void TwoKey();
	void ThreeKey();
	void FourKey();
	void FiveKey();
	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	void HighlightInventorySlot();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent,
	class AController * EventInstigator, AActor * DamageCauser );

	void Stun();

	void Die();
	UFUNCTION(BlueprintCallable)
	void FinishDeath();

private:     //------------------Private--------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float LookRate = 45.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float TurnRate = 45.0f;
	UPROPERTY(EditAnywhere)
	float RotRate = 540.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"))
	UAnimMontage* WeaponFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"))
	bool bISAiming = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"))
	float InterpSpeed = 20.0f;

	float CurrentFov;
	float DefaultFov;
	float ZoomedFov;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float HipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float HipLookRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float AimTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta= (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float AimLookRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CrossHairs, meta = (AllowPrivateAccess = "true"))
	float CrossHairSpreadMultiplier=0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CrossHairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor=0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CrossHairs, meta = (AllowPrivateAccess = "true"))
	float CrosshaitAimFactor=0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CrossHairs, meta = (AllowPrivateAccess = "true"))
	float CrosshaitInAirFactor=0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CrossHairs, meta = (AllowPrivateAccess = "true"))
	float CrosshaitShootingFactor=0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CrossHairs, meta = (AllowPrivateAccess = "true"))
	AItem* ItemActorLastFrame;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CrossHairs, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CrossHairs, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	bool GetBeamEndLocation(const FVector MuzzleSocketLocation,FHitResult& OutHitResult);
	
	bool bIsShooting = false;
	FTimerHandle BulletTimeHandle;

	bool bIsButtonPressed = false;
	bool bIsFiring = true;
	float FiringSpeed = 0.05;
	FTimerHandle FireTimehandle;

	FTimerHandle BurstTimerHandle;
	float BurstDelay;
	int32 BurstCount;
	int32 bIsSingle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta= (AllowPrivateAccess = "true"))
	USoundCue* WeaponSwitchModeSound;
	bool bShouldTrace;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta=(AllowPrivateAccess = "true"))
	bool bIsWeaponEquipped;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta=(AllowPrivateAccess = "true"))
	AItem* TraceHitItem;
	int8 OverlappedItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta= (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta= (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta= (AllowPrivateAccess = "true"))
	float CameraInterpDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta= (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta= (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Ammo, meta= (AllowPrivateAccess = "true"))
	int32 Starting9mmCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Ammo, meta= (AllowPrivateAccess = "true"))
	int32 StartingArCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Ammo, meta= (AllowPrivateAccess = "true"))
	ECombateState CombateState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Ammo, meta= (AllowPrivateAccess = "true"))
	EWeaponMode WeaponMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Movement, meta= (AllowPrivateAccess = "true"))
	bool bCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Movement, meta= (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Movement, meta= (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Movement, meta= (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Movement, meta= (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Movement, meta= (AllowPrivateAccess = "true"))
	float BaseGroundFriction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = Movement, meta= (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

	bool bAimingButtonPressed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	USceneComponent* AmmoInterpComp_1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	USceneComponent* AmmoInterpComp_2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	USceneComponent* AmmoInterpComp_3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	USceneComponent* AmmoInterpComp_4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	USceneComponent* AmmoInterpComp_5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SceneComponents, meta = (AllowPrivateAccess = "true"))
	USceneComponent* AmmoInterpComp_6;
	
	TArray<FInterpLocation> InterpLocations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;
	const int32 INVENTORY_CAPACITY{6};

	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	USoundCue* MeleeSoundCue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StunMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;
	
public:		//--------------------Public-----------------------

	FORCEINLINE USpringArmComponent* GetCameraBoom() const {return CameraBoom;}
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}
	FORCEINLINE bool GetIsAiming(){return bISAiming;}
	FORCEINLINE int8 GetOverlappedItemCount(){return OverlappedItemCount;}
	FORCEINLINE ECombateState GetCombateState(){return  CombateState;}
	FORCEINLINE bool GetCrouching(){return bCrouch;}
	FORCEINLINE AWeapon* GetEquippedWeapon(){return EquippedWeapon;}
	
	FORCEINLINE bool ShouldPlayEquipSound(){return bShouldPlayEquipSound;}
	FORCEINLINE bool ShouldPlayPickupSound(){return bShouldPlayPickupSound;}

	FORCEINLINE USoundCue* GetMeleeSoundCue(){return MeleeSoundCue;}
	FORCEINLINE UParticleSystem* GetBloodParticle(){return BloodParticle;}

	FORCEINLINE float GetStunChance(){return StunChance;}

	UFUNCTION(BlueprintCallable)
	float GetCrossHairSpreadMultiplier() const;
	void UpdateOverlappedItemCount(int8 Amount);
	void UnHighlightInventorySlot();
	
};

