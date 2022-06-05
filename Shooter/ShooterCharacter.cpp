// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Item.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Shooter.h"
#include "BulletHitInterface.h"
#include "Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
HipLookRate(1),
HipTurnRate(1),
AimLookRate(0.2),
AimTurnRate(0.2),
bIsSingle(1),
bShouldTrace(false),
OverlappedItemCount(0),
DefaultFov(0),
ZoomedFov(40),
bIsWeaponEquipped(true),
CameraInterpDistance(250.0f),
CameraInterpElevation(90.0f),
Starting9mmCount(90),
StartingArCount(120),
CombateState(ECombateState::ECS_Unoccupied),
WeaponMode(EWeaponMode::EWM_Auto),
BurstDelay(0.05),
BurstCount(3),
bCrouch(false),
BaseMovementSpeed(600),
CrouchMovementSpeed(300),
StandingCapsuleHalfHeight(88.0f),
CrouchingCapsuleHalfHeight(44.0f),
BaseGroundFriction(2.0f),
CrouchingGroundFriction(100.0f),
bAimingButtonPressed(false),
bShouldPlayEquipSound(true),
bShouldPlayPickupSound(true),
PickupSoundDelay(0.2f),
EquipSoundDelay(0.2f),
HighlightedSlot(-1),
MaxHealth(100.0f),
StunChance(0.25f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength=180.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 70.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0, RotRate, 0);

	GetCharacterMovement()->JumpZVelocity =300.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	HandSceneComponent=CreateDefaultSubobject<USceneComponent>("Hand Scene Component");

	WeaponInterpComp0 = CreateDefaultSubobject<USceneComponent>("Weapon Interp Comp");
	WeaponInterpComp0->SetupAttachment(GetFollowCamera());

	AmmoInterpComp_1 = CreateDefaultSubobject<USceneComponent>("Ammo Interp Comp 1");
	AmmoInterpComp_1->SetupAttachment(GetFollowCamera());
	AmmoInterpComp_2 = CreateDefaultSubobject<USceneComponent>("Ammo Interp Comp 2");
	AmmoInterpComp_2->SetupAttachment(GetFollowCamera());
	AmmoInterpComp_3 = CreateDefaultSubobject<USceneComponent>("Ammo Interp Comp 3");
	AmmoInterpComp_3->SetupAttachment(GetFollowCamera());
	AmmoInterpComp_4 = CreateDefaultSubobject<USceneComponent>("Ammo Interp Comp 4");
	AmmoInterpComp_4->SetupAttachment(GetFollowCamera());
	AmmoInterpComp_5 = CreateDefaultSubobject<USceneComponent>("Ammo Interp Comp 5");
	AmmoInterpComp_5->SetupAttachment(GetFollowCamera());
	AmmoInterpComp_6 = CreateDefaultSubobject<USceneComponent>("Ammo Interp Comp 6");
	AmmoInterpComp_6->SetupAttachment(GetFollowCamera());

	SwitchingMode();
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFov = FollowCamera->FieldOfView;
	CurrentFov = DefaultFov;
	
	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableGlow();
	EquippedWeapon->SetCharacter(this);

	InitializeAmmoMap();

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	InitializeInterpLocations();

	Health = MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimInterp(DeltaTime);
	GetScopeSensi();
	CalculateCrossHairSpread(DeltaTime);

	ItemTrace();
	InterpCapsuleHeight(DeltaTime);
}


void AShooterCharacter::MoveForward(float value)
{
	if((Controller!= nullptr) && (value!=0) )
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation{0, Rotation.Yaw, 0};

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, value);
	}
}

void AShooterCharacter::MoveLeft(float value)
{
	if((Controller!=nullptr)&&(value!=0))
	{
		FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRotation{0, Rotation.Yaw, 0};

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, value);
	}
	
}


// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveLeft", this, &AShooterCharacter::MoveLeft);
	
	PlayerInputComponent->BindAxis("MouseLook", this, &AShooterCharacter::MouseLook);
	PlayerInputComponent->BindAxis("MouseTurn", this, &AShooterCharacter::MouseTurn);

	// PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	// PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonStopped);
	
	PlayerInputComponent->BindAction("Scope", IE_Pressed, this, &AShooterCharacter::ScopePressed);
	PlayerInputComponent->BindAction("Scope", IE_Released, this, &AShooterCharacter::ScopeReleased);

	PlayerInputComponent->BindAction("Single", IE_Pressed, this, &AShooterCharacter::SwitchingMode);
	
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("Reload", IE_Released, this, &AShooterCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("Crouching", IE_Pressed, this, &AShooterCharacter::Crouch);

	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &AShooterCharacter::FKey);
	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &AShooterCharacter::OneKey);
	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &AShooterCharacter::TwoKey);
	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &AShooterCharacter::ThreeKey);
	PlayerInputComponent->BindAction("4Key", IE_Pressed, this, &AShooterCharacter::FourKey);
	PlayerInputComponent->BindAction("5Key", IE_Pressed, this, &AShooterCharacter::FiveKey);
}

void AShooterCharacter::Jump()
{
	if(!bCrouch)
	{
		ACharacter::Jump();
	}
	else
	{
		Crouch();
	}
}

void AShooterCharacter::FireWeapon()
{
	if(EquippedWeapon == nullptr){return;}
	if((CombateState!=ECombateState::ECS_Unoccupied)){return;}
	
	if(bIsAmmoLeft())
	{
		PlayFireSound();
		SendBullet();
		PlayMontage();
		EquippedWeapon->DecrementAmmo();
		AutoFireTimer();

		if(EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			EquippedWeapon->StartSlideTimer();
		}
	}

	StartCrossHairFire();
}

void AShooterCharacter::SingleFire()
{
	if(EquippedWeapon == nullptr){return;}
	if((CombateState!=ECombateState::ECS_Unoccupied)){return;}
	
	if(bIsAmmoLeft())
	{
		PlayFireSound();
		SendBullet();
		PlayMontage();
		EquippedWeapon->DecrementAmmo();
	}

	StartCrossHairFire();
	
}

void AShooterCharacter::Burst()
{
	if(EquippedWeapon == nullptr){return;}
	if((CombateState!=ECombateState::ECS_Unoccupied)){return;}

	
	if(bIsAmmoLeft())
	{
		PlayFireSound();
		SendBullet();
		PlayMontage();
		EquippedWeapon->DecrementAmmo();

		GetWorldTimerManager().SetTimer(BurstTimerHandle, this, &AShooterCharacter::EndBurst, BurstDelay);
		BurstCount--;
	}

	StartCrossHairFire();
	
}

void AShooterCharacter::EndBurst()
{
	if(BurstCount>0)
	{
		Burst();
	}
}


bool AShooterCharacter::GetBeamEndLocation(const FVector MuzzleSocketLocation, FHitResult& OutHitResult)
{
		FVector OutBeamEndPoint;
		FVector HitLocation;
		FHitResult HitResult;
	
		if(TraceUnderCrosshair(HitResult, HitLocation))
		{
			if(HitResult.bBlockingHit)
			{
				OutBeamEndPoint = HitLocation;
			}
		}

		FVector WeaponStart{MuzzleSocketLocation};
		FVector StartToEnd{OutBeamEndPoint - MuzzleSocketLocation};
		FVector WeaponEnd{WeaponStart + StartToEnd*1.25f};

		GetWorld()->LineTraceSingleByChannel(OutHitResult, WeaponStart, WeaponEnd, ECollisionChannel::ECC_Visibility);

		if(!OutHitResult.bBlockingHit)
		{
			OutHitResult.Location = OutBeamEndPoint;
			return false;
		}

	return true;
	
}

float AShooterCharacter::GetCrossHairSpreadMultiplier() const
{
	return CrossHairSpreadMultiplier;
}

void AShooterCharacter::AimInterp(float DeltaTime)
{
	if(bISAiming)
	{
		CurrentFov = FMath::FInterpTo(CurrentFov, ZoomedFov, DeltaTime, InterpSpeed);
	}
	else
	{
		CurrentFov = FMath::FInterpTo(CurrentFov, DefaultFov, DeltaTime, InterpSpeed);
	}
	FollowCamera->SetFieldOfView(CurrentFov);
}


void AShooterCharacter::ScopePressed()
{
	bAimingButtonPressed = true;
	if(CombateState != ECombateState::ECS_Reloading && CombateState != ECombateState::ECS_Equipping &&
		CombateState != ECombateState::ECS_Stunned)
	{
		Aiming();
	}
}

void AShooterCharacter::ScopeReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::GetScopeSensi()
{
	if(bISAiming)
	{
		LookRate = AimLookRate;
		TurnRate = AimTurnRate;
	}
	else
	{
		LookRate = HipLookRate;
		TurnRate = HipLookRate;
	}
}

void AShooterCharacter::MouseLook(float Value)
{
	AddControllerPitchInput(Value*TurnRate);
}

void AShooterCharacter::MouseTurn(float Value)
{
	AddControllerYawInput(Value*LookRate);
}

void AShooterCharacter::CalculateCrossHairSpread(float DeltaTime)
{
	FVector2D MaxSpeedRange{0,600.0f};
	FVector2D VelocityMultiplierRange{0.0, 1.0f};
	FVector Velocity{GetVelocity()};
	Velocity.Z=0;
	
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(MaxSpeedRange, VelocityMultiplierRange, Velocity.Size());

	if(GetCharacterMovement()->IsFalling())
	{
		CrosshaitInAirFactor = FMath::FInterpTo(CrosshaitInAirFactor, 2.25, DeltaTime, 5);
	}
	else
	{
		CrosshaitInAirFactor = FMath::FInterpTo(CrosshaitInAirFactor, 0, DeltaTime, 30);
	}

	if(bISAiming)
	{
		CrosshaitAimFactor = FMath::FInterpTo(CrosshaitAimFactor, 0.5, DeltaTime, 50);
	}
	else
	{
		CrosshaitAimFactor = FMath::FInterpTo(CrosshaitAimFactor, 0, DeltaTime, 50);
	}

	if(bIsFiring)
	{
		CrosshaitShootingFactor = FMath::FInterpTo(CrosshaitShootingFactor, 0.5, DeltaTime, 60);
	}
	else
	{
		CrosshaitShootingFactor = FMath::FInterpTo(CrosshaitShootingFactor, 0, DeltaTime, 60);
	}

	CrossHairSpreadMultiplier = 0.5 + CrosshairVelocityFactor + CrosshaitInAirFactor - CrosshaitAimFactor + CrosshaitShootingFactor;
}

void AShooterCharacter::AutoFireTimer()
{
	if(EquippedWeapon == nullptr){return;}
	CombateState = ECombateState::ECS_FireTimerInProgress;
	
	GetWorldTimerManager().SetTimer(BulletTimeHandle, this, &AShooterCharacter::AutoFire, EquippedWeapon->GetAutoFireRate());
}

void AShooterCharacter::AutoFire()
{
	if(CombateState == ECombateState::ECS_Stunned) return;
	
	CombateState = ECombateState::ECS_Unoccupied;
	if(EquippedWeapon == nullptr){return;}

	if(bIsAmmoLeft())
	{
		if(bIsButtonPressed && EquippedWeapon->GetAutomatic())
		{
			FireWeapon();
		}
	}
	else
	{
		Reload();
	}
}

void AShooterCharacter::FireButtonPressed()
{
	if(WeaponMode == EWeaponMode::EWM_Auto)
	{
		bIsButtonPressed = true;
		FireWeapon();
	}
	if(WeaponMode == EWeaponMode::EWM_Single)
	{
		bIsButtonPressed = true;
		SingleFire();
	}
	if(WeaponMode == EWeaponMode::EWM_Burst)
	{
		bIsButtonPressed = true;
		BurstCount = 3;
		Burst();
	}
	
}

void AShooterCharacter::FireButtonStopped()
{
	bIsButtonPressed = false;
}

void AShooterCharacter::StartCrossHairFire()
{
	
	bIsFiring = true;

	GetWorldTimerManager().SetTimer(FireTimehandle,this,&AShooterCharacter::FinishCrossHairFire,FiringSpeed);
}

void AShooterCharacter::FinishCrossHairFire()
{
	bIsFiring = false;
}

void AShooterCharacter::SwitchingMode()
{
	
	if(bIsSingle == 0)
	{
		WeaponMode = EWeaponMode::EWM_Single;
	}
	else if(bIsSingle == 1)
	{
		WeaponMode = EWeaponMode::EWM_Auto;
	}
	else if(bIsSingle == 2)
	{
		WeaponMode = EWeaponMode::EWM_Burst;
	}

	bIsSingle++;

	if(bIsSingle>=3)
	{
		bIsSingle = 0;
	}
	
	if(WeaponSwitchModeSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwitchModeSound);
	}
}

bool AShooterCharacter::TraceUnderCrosshair(FHitResult& OutHitResult, FVector& HitLocation)
{
	FVector2D ViewPort;

	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPort);
	}

	FVector WorldDirection;
	FVector WorldLocation;

	FVector2D CrosshairLocation{ViewPort.X/2.0f, ViewPort.Y/2.0f};

	CrosshairLocation.Y -=50.0f;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, WorldLocation, WorldDirection);

	if(bScreenToWorld)
	{
		FVector Start{WorldLocation};
		FVector End{WorldLocation + WorldDirection*50000};
		HitLocation = End;
		
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECC_Visibility);

		if(OutHitResult.bBlockingHit)
		{
			HitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
	
}

void AShooterCharacter::UpdateOverlappedItemCount(int8 Amount)
{
	if(OverlappedItemCount + Amount <=0)
	{
		OverlappedItemCount = 0;
		bShouldTrace = false;
	}
	else
	{
		OverlappedItemCount+=Amount;
		bShouldTrace = true;
	}
	
}

void AShooterCharacter::ItemTrace()
{
	if(bShouldTrace)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;

		if(TraceUnderCrosshair(ItemTraceResult, HitLocation))
		{
			if(ItemTraceResult.bBlockingHit)
			{
				TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);

				if(TraceHitItem)
				{
					if(HighlightedSlot == -1)
					{
						HighlightInventorySlot();
					}
				}
				else
				{
					if(HighlightedSlot!=-1)
					{
						UnHighlightInventorySlot();
					}
				}
				
				
				if(TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterp)
				{
					TraceHitItem = nullptr;
				}

				if(TraceHitItem && TraceHitItem->GetWidgetComponent())
				{
					TraceHitItem->GetWidgetComponent()->SetVisibility(true);
					TraceHitItem->EnableCustomDepth();

					if(Inventory.Num() >= INVENTORY_CAPACITY)
					{
						TraceHitItem->SetInventoryFull(true);
					}
					else
					{
						TraceHitItem->SetInventoryFull(false);
					}
				}
				
				if(ItemActorLastFrame)
				{
					if(TraceHitItem!=ItemActorLastFrame)
					{
						ItemActorLastFrame->GetWidgetComponent()->SetVisibility(false);
						ItemActorLastFrame->DisableCustomDepth();
					}
				}

				ItemActorLastFrame = TraceHitItem;
			}
		}
	}
	else if(ItemActorLastFrame)
	{
		ItemActorLastFrame->GetWidgetComponent()->SetVisibility(false);
		if(TraceHitItem)
		{
			TraceHitItem->DisableCustomDepth();
		}
	}
		
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if(DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* Weapon, bool bswapping)
{
	if(Weapon)
	{
		const USkeletalMeshSocket* SkeletonMesh = GetMesh()->GetSocketByName("RightHandSocket");

		if(SkeletonMesh)
		{
			SkeletonMesh->AttachActor(Weapon, GetMesh());
		}

		if(EquippedWeapon == nullptr)
		{
			EquipItemDelegate.Broadcast(-1, Weapon->GetSlotIndex());
		}
		else if(!bswapping)
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), Weapon->GetSlotIndex());
		}

		EquippedWeapon = Weapon;
		Weapon->SetItemState(EItemState::EIS_Equipped);
		bIsWeaponEquipped = true;
	}
}

void AShooterCharacter::DropWeapon()
{
	if(EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules{EDetachmentRule::KeepWorld, true};
		
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetItemProperties(EItemState::EIS_Falling);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->Throw();
		bIsWeaponEquipped = false;

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue, FString::Printf(TEXT("Weapon : %s"), *EquippedWeapon->GetName()));
		}
	}
}

void AShooterCharacter::SelectButtonPressed()
{
	if(CombateState != ECombateState::ECS_Unoccupied) return;
	if(TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr;
	}
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::Swap(AWeapon* Weapon)
{
	if(Inventory.Num()-1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = Weapon;
		Weapon->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}
	DropWeapon();
	EquipWeapon(Weapon, true);
	TraceHitItem = nullptr;
	ItemActorLastFrame = nullptr;
}

void AShooterCharacter::GetPickUpItem(AItem* Item)
{
	Item->PlayEquipSound();

	auto Weapon = Cast<AWeapon>(Item);
	if(Weapon)
	{
		if(Inventory.Num()<INVENTORY_CAPACITY)
		{
			Weapon->SetItemState(EItemState::EIS_PickedUp);
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
		}
		else
		{
			Swap(Weapon);
		}
	}
	
	auto Ammo = Cast<AAmmo>(Item);
	if(Ammo)
	{
		PickUpAmmo(Ammo);
	}
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmCount);
	AmmoMap.Add(EAmmoType::EAT_Ar, StartingArCount);
}

bool AShooterCharacter::bIsAmmoLeft()
{
	if(EquippedWeapon == nullptr){return false;}

	return EquippedWeapon->GetAmmo()>0;
}

void AShooterCharacter::PlayFireSound()
{
	if(EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SendBullet()
{
	const USkeletalMeshSocket* MuzzleSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("Muzzle");
	if(MuzzleSocket)
	{
		const FTransform MuzzleTransform =  MuzzleSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		if(EquippedWeapon->GetMuzzleFlash())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), MuzzleTransform);
		}

		FHitResult BeamEndHitResult;
		
		if(GetBeamEndLocation(MuzzleTransform.GetLocation(), BeamEndHitResult))
		{
			if(BeamEndHitResult.Actor.IsValid())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamEndHitResult.Actor.Get());
				if(BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamEndHitResult, this, GetController());
				}
				AEnemy* Enemy = Cast<AEnemy>(BeamEndHitResult.Actor.Get());
				if(Enemy)
				{
					float Damage;
					if(BeamEndHitResult.BoneName.ToString() == Enemy->GetHeadBoneName())
					{
						Damage = EquippedWeapon->GetHeadDamage();
						UGameplayStatics::ApplyDamage(BeamEndHitResult.Actor.Get(), Damage,
							GetController(), this, UDamageType::StaticClass());
						Enemy->ShowHitNumbers(Damage, BeamEndHitResult.Location, true);
					}
					else
					{
						Damage = EquippedWeapon->GetDamage();
						UGameplayStatics::ApplyDamage(BeamEndHitResult.Actor.Get(), Damage,
							GetController(), this, UDamageType::StaticClass());
						Enemy->ShowHitNumbers(Damage, BeamEndHitResult.Location, false);
					}
				}
			}
			else
			{
				if(ImpactParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, BeamEndHitResult.Location);
				}
			}
			if(BeamParticle)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle,MuzzleTransform);
				if(Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEndHitResult.Location);
				}
			}
		}
	}
}

void AShooterCharacter::PlayMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); 
	if(AnimInstance && WeaponFire)
	{
		AnimInstance->Montage_Play(WeaponFire);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	Reload();
}

void AShooterCharacter::Reload()
{
	if(CombateState!=ECombateState::ECS_Unoccupied){return;}
	if(EquippedWeapon == nullptr){return;}
	
	if(CarryingAmmo() && !EquippedWeapon->bIsClipFull())
	{
		if(bISAiming)
		{
			StopAiming();
		}
		
		CombateState = ECombateState::ECS_Reloading;
		
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance&&ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSectionName());
		}
	}
}

void AShooterCharacter::FinishReload()
{
	if(CombateState == ECombateState::ECS_Stunned) return;
	
	CombateState = ECombateState::ECS_Unoccupied;

	if(EquippedWeapon==nullptr){return;}

	if(bAimingButtonPressed)
	{
		Aiming();
	}

	const auto AmmoType = EquippedWeapon->GetAmmoType();
	
	if(AmmoMap.Contains(AmmoType))
	{
		int32 CarriedAmmo = AmmoMap[AmmoType];

		int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if(MagEmptySpace>CarriedAmmo)
		{
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo-=MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

bool AShooterCharacter::CarryingAmmo()
{
	if(EquippedWeapon == nullptr){return false;}

	EAmmoType AmmoType = EquippedWeapon->GetAmmoType();

	if(AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
		
}

void AShooterCharacter::GrabClip()
{
	if(EquippedWeapon == nullptr){return;}
	if(HandSceneComponent == nullptr){return;}
	
	int32 WeaponBoneIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName());
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(WeaponBoneIndex);

	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentTransformRules, FName("Hand_l"));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	EquippedWeapon->SetIsMoving(true);
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetIsMoving(false);
	
}

void AShooterCharacter::Crouch()
{
	if(!GetCharacterMovement()->IsFalling())
	{
		bCrouch = !bCrouch;
	}

	if(bCrouch)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::InterpCapsuleHeight(float DeltaTime)
{
	float TargetHeight;

	if(bCrouch)
	{
		TargetHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetHeight = StandingCapsuleHalfHeight;
	}

	float InterpHeight{FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetHeight, DeltaTime, 10.0f)};

	//+ve for standing -ve for crouching
	float DeltaCapsuleHeight  = InterpHeight-GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector MeshOffset{0,0,-DeltaCapsuleHeight};
	GetMesh()->AddLocalOffset(MeshOffset);
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHeight);
}

void AShooterCharacter::Aiming()
{
	bISAiming = true;

	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::StopAiming()
{
	bISAiming = false;

	if(!bCrouch)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::PickUpAmmo(AAmmo* Ammo)
{
	if(AmmoMap.Find(Ammo->GetAmmoType()))
	{
		int32 AmmoCount{AmmoMap[Ammo->GetAmmoType()]};
		AmmoCount+=Ammo->GetAmmoCount();
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if(EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if(EquippedWeapon->GetAmmo() == 0)
		{
			Reload();
		}
	}

	Ammo->Destroy();
}

int32 AShooterCharacter::GetEmptyInventorySlots()
{
	for(int32 i=0; i<Inventory.Num(); i++)
	{
		if(Inventory[i] == nullptr)
			return i;
	}
	if(Inventory.Num() < INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}

	return -1;
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{WeaponInterpComp0, 0};
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{AmmoInterpComp_1, 0};
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{AmmoInterpComp_2, 0};
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{AmmoInterpComp_3, 0};
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{AmmoInterpComp_4, 0};
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{AmmoInterpComp_5, 0};
	InterpLocations.Add(InterpLoc5);
	
	FInterpLocation InterpLoc6{AmmoInterpComp_6, 0};
	InterpLocations.Add(InterpLoc6);
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}
	return FInterpLocation();
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 lowestIndex = 1;
	int32 lowestCount = INT_MAX;

	for(int32 i = 1; i<InterpLocations.Num(); i++)
	{
		if(InterpLocations[i].ItemCount < lowestCount)
		{
			lowestIndex = i;
			lowestCount = InterpLocations[i].ItemCount;
		}
	}
	return lowestIndex;
}

void AShooterCharacter::IncrementItemCount(int32 Index, int32 Amount)
{
	if(Amount < -1 || Amount > 1){return;}
	
	if(InterpLocations.Num()>=Index)
	{
		InterpLocations[Index].ItemCount+=Amount;
	}
}

void AShooterCharacter::ResetEquipSound()
{
	bShouldPlayEquipSound = true;
}

void AShooterCharacter::ResetPickupSound()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::StartPickupSound()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundHandler, this, &AShooterCharacter::ResetPickupSound, PickupSoundDelay);
}

void AShooterCharacter::StartEquipSound()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundHandler, this, &AShooterCharacter::ResetEquipSound, EquipSoundDelay);
}

void AShooterCharacter::FKey()
{
	if(EquippedWeapon->GetSlotIndex() == 0){return;}

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::OneKey()
{
	if(EquippedWeapon->GetSlotIndex() == 1){return;}

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void AShooterCharacter::TwoKey()
{
	if(EquippedWeapon->GetSlotIndex() == 2){return;}

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AShooterCharacter::ThreeKey()
{
	if(EquippedWeapon->GetSlotIndex() == 3){return;}

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AShooterCharacter::FourKey()
{
	if(EquippedWeapon->GetSlotIndex() == 4){return;}

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void AShooterCharacter::FiveKey()
{
	if(EquippedWeapon->GetSlotIndex() == 5){return;}

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

void AShooterCharacter::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if(CurrentItemIndex != NewItemIndex && NewItemIndex < Inventory.Num() && (CombateState==ECombateState::ECS_Unoccupied || CombateState == ECombateState::ECS_Equipping)) 
	{
		if(bISAiming)
		{
			StopAiming();
		}
		
		auto OldEquippedWeapon = EquippedWeapon;
		auto NewEquippedWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewEquippedWeapon);

		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		NewEquippedWeapon->SetItemState(EItemState::EIS_Equipped);

		CombateState = ECombateState::ECS_Equipping;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
		}

		NewEquippedWeapon->PlayEquipSound(true);
	}
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot{GetEmptyInventorySlots()};
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
}

void AShooterCharacter::FinishEquipping()
{
	if(CombateState == ECombateState::ECS_Stunned) return;
	
	CombateState = ECombateState::ECS_Unoccupied;
	if(bAimingButtonPressed)
	{
		Aiming();
	}
}

EPhysicalSurface AShooterCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	FVector Start{GetActorLocation()};
	FVector End{Start + FVector(0,0,-400.0f)};
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(Health - DamageAmount <= 0)
	{
		Health = 0;
		Die();

		AEnemyAIController* EC = Cast<AEnemyAIController>(EventInstigator);
		if(EC)
		{
			EC->GetBlackboardComponent()->SetValueAsBool(FName("CharacterDeath"), true);
		}
	}
	else
	{

		Health-=DamageAmount;
	}

	return DamageAmount;
}

void AShooterCharacter::Stun()
{
	if(Health<=0) return;
	
	CombateState = ECombateState::ECS_Stunned;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && StunMontage)
	{
		AnimInstance->Montage_Play(StunMontage);
	}
}

void AShooterCharacter::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
}

void AShooterCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if(PC)
	{
		DisableInput(PC);
	}
}

void AShooterCharacter::EndStun()
{
	CombateState = ECombateState::ECS_Unoccupied;

	if(bAimingButtonPressed)
	{
		Aiming();
	}
}

