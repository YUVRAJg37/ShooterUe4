// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()

public:

	AAmmo();

	virtual void Tick(float DeltaSeconds) override;

protected:

	virtual void BeginPlay() override;

	virtual void SetItemProperties(EItemState ItemProp) override;

	UFUNCTION()
	void OnAmmoBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoTexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	USphereComponent* AmmoCollisionSphere;

public:

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh(){return AmmoMesh;}
	FORCEINLINE EAmmoType GetAmmoType(){return AmmoType;}

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
	virtual void InitializeCustomDepth() override;
	
};

