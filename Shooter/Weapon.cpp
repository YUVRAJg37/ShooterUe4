// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon() :
bIsFalling(false),
Timer(0.7f),
Ammo(30),
MagazineCapacity(30),
AmmoType(EAmmoType::EAT_9mm),
WeaponType(EWeaponType::EWT_SubmachineGun),
ReloadMontageSectionName("Reload"),
ClipBoneName(TEXT("smg_clip")),
bSliding(false),
SlideTime(0.1),
SlideDisplacement(0),
MaxSlideDisplacement(4.0f),
MaxRecoilRotation(20.0f),
RecoilRotation(0),
bAutomatic(true)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(BoneToHide != "")
	{
		GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
	}
	
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(bIsFalling && (GetItemState() == EItemState::EIS_Falling))
	{
		FRotator MeshRotation{0, GetItemMesh()->GetComponentRotation().Yaw, 0};
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	UpdateSlideDisplacement();
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString WeaponTablePath = TEXT("DataTable'/Game/_Game/DataTables/DT_Weapon.DT_Weapon'");
	UDataTable* DataTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(),nullptr, *WeaponTablePath));

	if(DataTableObject)
	{
	
		FWeaponDataTable* WeaponDataRow = nullptr; 
		
		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachineGun :
			{
				WeaponDataRow = DataTableObject->FindRow<FWeaponDataTable>(FName("SubMachineGun"), TEXT(""));
				break;
			}
		case EWeaponType::EWT_AssaultRifle :
			{
				WeaponDataRow = DataTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
				break;
			}
		case EWeaponType::EWT_Pistol :
			{
				WeaponDataRow = DataTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));
				break;
			}
		}

		if(WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazineCapacity;
			SetEquipUpSound(WeaponDataRow->EquipSound);
			SetPickUpSound(WeaponDataRow->PickupSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetInventoryIcon(WeaponDataRow->InventoryIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);
			SetMaterialInstance(WeaponDataRow->MaterialInstance);
			SetClipBoneName(WeaponDataRow->ClipBoneName);
			SetReloadMontageSectionName(WeaponDataRow->ReloadMontageSectionName);
			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBp);
			CrossHairMiddle = WeaponDataRow->CrossHairMiddle;
			CrossHairTop = WeaponDataRow->CrossHairTop;
			CrossHairBottom = WeaponDataRow->CrossHairBottom;
			CrossHairLeft = WeaponDataRow->CrossHairLeft;
			CrossHairRight = WeaponDataRow->CrossHairRight;
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			FireSound = WeaponDataRow->FireSound;
			BoneToHide = WeaponDataRow->BoneToHide;
			bAutomatic = WeaponDataRow->bAutomatic;
			Damage = WeaponDataRow->Damage;
			HeadDamage = WeaponDataRow->HeadDamage;

			PreviousMaterialIndex = GetMaterialIndex();
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
			
			SetMaterialIndex(WeaponDataRow->MaterialIndex);
		}

		if(GetMaterialInstance())
		{
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
			GetDynamicMaterialInstance()->SetVectorParameterValue(FName("FresnelGlowParameter"), GetGlowColour());
			GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());
			EnableGlow();
		}
		
	}
}

void AWeapon::Throw()
{
	FRotator MeshRotation{0, GetItemMesh()->GetComponentRotation().Yaw, 0};
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	FVector MeshRight{GetItemMesh()->GetRightVector()};
	FVector MeshForward{GetItemMesh()->GetForwardVector()};
	
	MeshRight = MeshRight.RotateAngleAxis(20, MeshForward);
	MeshRight = MeshRight.RotateAngleAxis(30 , FVector(0,0,1));
	
	MeshRight*=5000;
	
	GetItemMesh()->AddImpulse(MeshRight);

	bIsFalling = true;

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWeapon::NotFalling, Timer);

	EnableGlow();
	
}

void AWeapon::NotFalling()
{
	bIsFalling = false;
	SetItemState(EItemState::EIS_PickUp);
	StartPulseTimer();
}

void AWeapon::DecrementAmmo()
{
	if(Ammo-1<=0)
	{
		Ammo = 0;
	}
	else
	{
		Ammo--;
	}
	
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("Attempted to reload more thn magazine capacity"));
	Ammo+=Amount;
}

bool AWeapon::bIsClipFull()
{
	return Ammo>=MagazineCapacity;
}

void AWeapon::FinishSliding()
{
	bSliding = false;
}

void AWeapon::StartSlideTimer()
{
	bSliding = true;
	GetWorldTimerManager().SetTimer(SlideTimerHandle, this, &AWeapon::FinishSliding, SlideTime);
}

void AWeapon::UpdateSlideDisplacement()
{
	if(SlideDisplacementCurve && bSliding)
	{
		const float Elapsedtime = GetWorldTimerManager().GetTimerElapsed(SlideTimerHandle);
		const float CurveValue = SlideDisplacementCurve->GetFloatValue(Elapsedtime);
		SlideDisplacement = CurveValue * MaxSlideDisplacement;
		RecoilRotation = CurveValue * MaxRecoilRotation;
	}
}


