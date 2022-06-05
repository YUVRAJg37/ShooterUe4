// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoType.h"
#include "Item.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ClipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSectionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairMiddle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairTop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrossHairBottom;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadDamage;
};

UCLASS()
class SHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

	public:

	AWeapon();

	void Throw();

	void NotFalling();

	

	private:
	
	float Timer;
	FTimerHandle TimerHandle;
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapacity;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSectionName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bISMoving;
	FName ClipBoneName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;
	int32 PreviousMaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairMiddle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairTop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrossHairBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	float AutoFireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	USoundCue* FireSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = "Data Table", meta = (AllowPrivateAccess = "true"))
	FName BoneToHide;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	float SlideDisplacement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SlideDisplacementCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	bool bSliding;
	FTimerHandle SlideTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	float SlideTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	float MaxSlideDisplacement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	float MaxRecoilRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	float RecoilRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	bool bAutomatic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowPrivateAccess = "true"))
	float HeadDamage;
	
	protected:

	virtual void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void OnConstruction(const FTransform& Transform) override;

	void FinishSliding();
	void UpdateSlideDisplacement();
	
public:

	FORCEINLINE int32 GetAmmo(){return Ammo;}
	FORCEINLINE int32 GetMagazineCapacity(){return MagazineCapacity;}
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	FORCEINLINE EAmmoType GetAmmoType() const {return AmmoType;}
	FORCEINLINE FName GetReloadMontageSectionName() const {return ReloadMontageSectionName;}
	FORCEINLINE void SetReloadMontageSectionName(FName name){ReloadMontageSectionName = name;}
	FORCEINLINE FName GetClipBoneName() const {return ClipBoneName;}
	FORCEINLINE void SetClipBoneName(FName name) {ClipBoneName = name;}
	FORCEINLINE void SetIsMoving(bool move) {bISMoving = move;}
	FORCEINLINE float GetAutoFireRate(){return AutoFireRate;}
	FORCEINLINE UParticleSystem* GetMuzzleFlash(){return MuzzleFlash;}
	FORCEINLINE USoundCue* GetFireSound(){return FireSound;}
	FORCEINLINE bool GetAutomatic(){return bAutomatic;}
	FORCEINLINE float GetDamage(){return Damage;}
	FORCEINLINE float GetHeadDamage(){return HeadDamage;}

	bool bIsClipFull();
	
	void DecrementAmmo();
	void ReloadAmmo(int32 Amount);

	void StartSlideTimer();
};
