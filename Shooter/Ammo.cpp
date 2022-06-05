// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "ShooterCharacter.h"

AAmmo::AAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ammo Mesh");
	SetRootComponent(AmmoMesh);

	GetWidgetComponent()->SetupAttachment(GetRootComponent());
	GetSphereComponent()->SetupAttachment(GetRootComponent());
	GetBoxComponent()->SetupAttachment(GetRootComponent());

	AmmoCollisionSphere = CreateDefaultSubobject<USphereComponent>("Ammo Collision Sphere");
	AmmoCollisionSphere->SetupAttachment(GetRootComponent());
	AmmoCollisionSphere->SetSphereRadius(50.0f);
}

void AAmmo::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	AmmoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnAmmoBeginOverlap);
	
	InitializeCustomDepth();
}

void AAmmo::SetItemProperties(EItemState ItemProp)
{
	Super::SetItemProperties(ItemProp);

	switch (ItemProp)
	{  
		case EItemState::EIS_PickUp :
			{
				AmmoMesh->SetSimulatePhysics(false);
				AmmoMesh->SetEnableGravity(false);
				AmmoMesh->SetVisibility(true);
				AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
				break;
			}
		case  EItemState::EIS_Equipped :
			{
				AmmoMesh->SetSimulatePhysics(false);
				AmmoMesh->SetEnableGravity(false);
				AmmoMesh->SetVisibility(true);
				AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				break;
			}
		case EItemState::EIS_Falling :
			{
				AmmoMesh->SetSimulatePhysics(true);
				AmmoMesh->SetEnableGravity(true);
				AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

				break;
			}
		case EItemState::EIS_EquipInterp:
			{
			
				AmmoMesh->SetSimulatePhysics(false);
				AmmoMesh->SetEnableGravity(true);
				AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

				break;
			}

		default:
			{
				break;
			}
	}
}

void AAmmo::OnAmmoBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Other)
	{
		AShooterCharacter* character = Cast<AShooterCharacter>(Other);
		if(character)
		{
			StartItemCurve(character);
			AmmoCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AAmmo::EnableCustomDepth()
{
	AmmoMesh->SetRenderCustomDepth(true);
}

void AAmmo::DisableCustomDepth()
{
	AmmoMesh->SetRenderCustomDepth(false);
}

void AAmmo::InitializeCustomDepth()
{
	AmmoMesh->SetRenderCustomDepth(false);
}
