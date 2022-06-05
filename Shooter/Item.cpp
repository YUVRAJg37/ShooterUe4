// Fill out your copyright notice in the Description page of Project Settings.



#include "Item.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AItem::AItem() :
ItemRarity(EItemRarity::EIR_Common),
ItemState(EItemState::EIS_PickUp),
InterpTimer(0.7f),
InterpStartLocation(FVector(0)),
CameraTargetLocation((FVector(0))),
ItemType(EItemType::EIT_MAX),
InterpLocationIndex(0),
MaterialIndex(0),
bCanChangeCustomDepth(true),
//Dynamic Material Parameters
GlowAmount(150.0f),
FresnelExponent(3.f),
FresnelReflectionfactor(4.f),
PulseCurveTime(5.f),
SlotIndex(0),
bISInventoryFull(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Item Mesh");
	SetRootComponent(ItemMesh);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Box Component");
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>("PickUp Widget");
	PickUpWidget->SetupAttachment(GetRootComponent());

	AreaSphere=CreateDefaultSubobject<USphereComponent>("Area Sphere");
	AreaSphere->SetupAttachment(GetRootComponent());

}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	FString RarityTablePath(TEXT("DataTable'/Game/_Game/DataTables/DT_ItemRarity.DT_ItemRarity'"));
	UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityTablePath));

	if(RarityTableObject)
	{
		FItemRarityDataTable* RarityRow = nullptr;

		switch (ItemRarity)
		{
		case EItemRarity::EIR_Damaged :
			{
				RarityRow = RarityTableObject->FindRow<FItemRarityDataTable>(FName("Damaged"), TEXT(""));
				break;
			}

		case EItemRarity::EIR_Common :
			{
	
				RarityRow = RarityTableObject->FindRow<FItemRarityDataTable>(FName("Common"), TEXT(""));
				break;
			}
		case EItemRarity::EIR_UnCommon :
			{
				RarityRow = RarityTableObject->FindRow<FItemRarityDataTable>(FName("Uncommon"), TEXT(""));
				break;
			}

		case EItemRarity::EIR_Rare :
			{
				RarityRow = RarityTableObject->FindRow<FItemRarityDataTable>(FName("Rare"), TEXT(""));
				break;
			}

		case EItemRarity::EIR_Legendary :
			{
				RarityRow = RarityTableObject->FindRow<FItemRarityDataTable>(FName("Legendary"), TEXT(""));
				break;
			}
		}

		if(RarityRow)
		{
			GlowColour = RarityRow->GlowColour;
			DarkColour = RarityRow->DarkColour;
			LightColour = RarityRow->LightColour;
			NoOfStars = RarityRow->NumberOfStars;
			ItemBackground = RarityRow->IconBackground;
			if(ItemMesh)
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencilValue);
			}
		}
		
		if(MaterialInstance)
		{
			DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
			DynamicMaterialInstance->SetVectorParameterValue(FName("FresnelGlowParameter"), GlowColour);
			ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
			EnableGlow();
		}

		
	}
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	PickUpWidget->SetVisibility(false);

	SetStars();

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	SetItemState(ItemState);

	InitializeCustomDepth();

	StartPulseTimer();
}



// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ItemInterp(DeltaTime);
	UpdatePulse();
}


void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Other)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Other);
		{
			if(ShooterCharacter)
			{
				ShooterCharacter->UpdateOverlappedItemCount(1);
			}
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(Other)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(Other);
		{
			if(ShooterCharacter)
			{
				ShooterCharacter->UpdateOverlappedItemCount(-1);
				ShooterCharacter->UnHighlightInventorySlot();
			}
		}
	}
}



void AItem::SetStars()
{
	for(int8 i =0 ; i<=5; i++)
	{
		ItemStars.Add(false);
	}

	switch (ItemRarity)
	{
		case EItemRarity::EIR_Damaged : {
				ItemStars[1] = true;
				break;
		}
	case EItemRarity::EIR_Common : {
				ItemStars[1] = true;
				ItemStars[2] = true;
				break;
	}
	case EItemRarity::EIR_UnCommon : {
				ItemStars[1] = true;
				ItemStars[2] = true;
				ItemStars[3] = true;
				break;
	}
	case EItemRarity::EIR_Rare : {
				ItemStars[1] = true;
				ItemStars[2] = true;
				ItemStars[3] = true;
				ItemStars[4] = true;
				break;
	}
	case EItemRarity::EIR_Legendary : {
				ItemStars[1] = true;
				ItemStars[2] = true;
				ItemStars[3] = true;
				ItemStars[4] = true;
				ItemStars[5] = true;
				break;
	}
	}
}

void AItem::SetItemProperties(EItemState ItemProp)
{
	switch (ItemProp)
	{
		case EItemState::EIS_PickUp :
			{
				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetEnableGravity(false);
				ItemMesh->SetVisibility(true);
				ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

				CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
				CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				

				break;
			}
		case  EItemState::EIS_Equipped :
			{
				PickUpWidget->SetVisibility(false);
				
				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetEnableGravity(false);
				ItemMesh->SetVisibility(true);
				ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
				break;
			}
		case EItemState::EIS_Falling :
			{
				ItemMesh->SetSimulatePhysics(true);
				ItemMesh->SetEnableGravity(true);
				ItemMesh->SetVisibility(true);
				ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
				
				AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				break;
			}
		case EItemState::EIS_EquipInterp:
			{
				GetWidgetComponent()->SetVisibility(false);
				
				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetEnableGravity(true);
				ItemMesh->SetVisibility(true);
				ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
				
				AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
				CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				break;
			}
	case EItemState::EIS_PickedUp:
			{
				GetWidgetComponent()->SetVisibility(false);

				ItemMesh->SetVisibility(false);
				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetEnableGravity(true);
				ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
				
				AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
				CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
				break;
			}
	}
}

void AItem::SetItemState(EItemState SetItemState)
{
	ItemState = SetItemState;
	SetItemProperties(ItemState);
}

void AItem::StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound)
{
	Character = Char;

	InterpLocationIndex = Character->GetInterpLocationIndex();
	Character->IncrementItemCount(InterpLocationIndex, 1);

	GetWorldTimerManager().ClearTimer(PulseTimerHandle);
	
	bIsInterping = true;
	InterpStartLocation = GetActorLocation();
	SetItemState(EItemState::EIS_EquipInterp);

	PlayPickupSound(bForcePlaySound);
	
	if(Character)
	{
		GetWorldTimerManager().SetTimer(InterpTimerHandle, this, &AItem::FinishInterping, InterpTimer, false);
	}

	float CameraInterpYaw = Character->GetFollowCamera()->GetComponentRotation().Yaw;
	float WeaponInterpYaw = GetActorRotation().Yaw;

	RotateOffset = WeaponInterpYaw - CameraInterpYaw;

	bCanChangeCustomDepth = false;
}

void AItem::FinishInterping()
{
	bIsInterping = false;

	Character->IncrementItemCount(InterpLocationIndex, -1);
	
	if(Character)
	{
		Character->GetPickUpItem(this);
		SetActorScale3D(FVector(1.0f));
		Character->UnHighlightInventorySlot();
	}
	
	bCanChangeCustomDepth = true;
	DisableGlow();
	DisableCustomDepth();
	
}

void AItem::ItemInterp(float DeltaTime)
{
	if(!bIsInterping){return;}
	
	if(Character && ItemZCurve)
	{
		FVector ItemLocation = InterpStartLocation;
		FVector CameraInterpLocation = GetInterpLocation();
		
		float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(InterpTimerHandle);
		float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
	
		float ScaleFactor{FVector(0.0f, 0.0f, (ItemLocation - CameraInterpLocation).Z).Size()};

		FVector CurrentLocation{GetActorLocation()};

		float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.0f);
		float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.0f);

		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;
		
		ItemLocation.Z+=CurveValue*ScaleFactor;
		SetActorLocation(ItemLocation, false, nullptr, ETeleportType::TeleportPhysics);

		FRotator ItemRotation{0.0f, Character->GetFollowCamera()->GetComponentRotation().Yaw + RotateOffset, 0.0f};

		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if(ItemScaleCurve)
		{
			float Scale = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(Scale, Scale, Scale));
		}
	}
}

FVector AItem::GetInterpLocation()
{
	if(Character == nullptr){return FVector{0};}
	
	switch (ItemType)
	{
	case EItemType::EIT_Ammo :
		{
			return  Character->GetInterpLocation(InterpLocationIndex).SceneComponents->GetComponentLocation();
			break;
		}

	case EItemType::EIT_Weapon:
		return Character->GetInterpLocation(0).SceneComponents->GetComponentLocation();
		break;
	}

	return FVector();
}

void AItem::PlayPickupSound(bool bForcePlaySound)
{
	if(Character)
	{
		if(bForcePlaySound)
		{
			if(PickUpSound)
			{
				UGameplayStatics::PlaySound2D(this, PickUpSound);
			}
		}
		else if(Character->ShouldPlayPickupSound())
		{
			Character->StartPickupSound();
			if(PickUpSound)
			{
				UGameplayStatics::PlaySound2D(this, PickUpSound);
			}
		}
	}
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if(Character)
	{
		if(bForcePlaySound)
		{
			if(PickUpSound)
			{
				UGameplayStatics::PlaySound2D(this, PickUpSound);
			}
		}
		else if(Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSound();
			if(EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

void AItem::EnableCustomDepth()
{
	if(bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(true);
	}
}

void AItem::DisableCustomDepth()
{
	if(bCanChangeCustomDepth && ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(false);
}

void AItem::EnableGlow()
{
	if(DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void AItem::DisableGlow()
{
	if(DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}
}

void AItem::StartPulseTimer()
{
	if(ItemState == EItemState::EIS_PickUp)
	{
		GetWorldTimerManager().SetTimer(PulseTimerHandle, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::UpdatePulse()
{

	if(!PulseCurve){return;}
	if(!InterpPulseCurve){return;}
	
	float ElapsedTime;
	FVector CurveValue;
	
	switch (ItemState)
	{
	case EItemState::EIS_PickUp :
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimerHandle);
			CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
			break;
		}
	case EItemState::EIS_EquipInterp :
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(InterpTimerHandle);
			CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
			break;
		}
	}
	
	if(DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue("GlowAmount", CurveValue.X*GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue("FresnelExponentFactor", CurveValue.Y*FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue("FresnelBaseReflectFactor", CurveValue.Z*FresnelReflectionfactor);
	}
}

