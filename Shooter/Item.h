// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Curves/CurveVector.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include  "WeaponType.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_UnCommon UMETA(DisplayName = "UnCommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "MaxDefault")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	
	EIT_MAX UMETA(DisplayName = "MaxDefault")
};

UENUM(BlueprintType)
enum class EItemState: uint8
{
	EIS_PickUp UMETA(DisplayName = "PickUp"),
	EIS_EquipInterp UMETA(DisplayName = "EquipInterp"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
	
	EIR_MAX UMETA(DisplayName = "MaxDefault")
};

USTRUCT(BlueprintType)
struct FItemRarityDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencilValue;
	
};

UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void FinishInterping();
	void ItemInterp(float DeltaTime);
	FVector GetInterpLocation();

	void UpdatePulse();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetStars();

	virtual void SetItemProperties(EItemState ItemProp);
	
	private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName{"Default"};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 AmmoCount{0};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rarity", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ItemStars;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true") )
	UCurveFloat* ItemZCurve;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true") )
	UCurveFloat* ItemScaleCurve;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true") )
	FVector InterpStartLocation;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true") )
	FVector CameraTargetLocation;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true") )
	FTimerHandle InterpTimerHandle;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true") )
	float InterpTimer;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true") )
	bool bIsInterping;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true") )
	class AShooterCharacter* Character;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	USoundCue* PickUpSound;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true") )
	USoundCue* EquipSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickUpWidget;
	
	int32 InterpLocationIndex;
	float RotateOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	bool bCanChangeCustomDepth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Pulse, meta = (AllowPrivateAccess = "true"))
	UCurveVector* PulseCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Pulse, meta = (AllowPrivateAccess = "true"))
	UCurveVector* InterpPulseCurve;
	FTimerHandle PulseTimerHandle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Pulse, meta = (AllowPrivateAccess = "true"))
	float PulseCurveTime;
	float GlowAmount;
	float FresnelExponent;
	float FresnelReflectionfactor;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Background, meta = (AllowPrivateAccess = "true"))
	// UTexture2D* ItemBackground;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	bool bISInventoryFull;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemRarityDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	FLinearColor GlowColour;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	FLinearColor DarkColour;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	FLinearColor LightColour;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	int32 NoOfStars;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemBackground;
	
	public:

	FORCEINLINE UWidgetComponent* GetWidgetComponent() const { return PickUpWidget;}
	FORCEINLINE USphereComponent* GetSphereComponent() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetBoxComponent() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState;}
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const {return ItemMesh;}
	FORCEINLINE USoundCue* GetPickUpSound() { return PickUpSound;}
	FORCEINLINE USoundCue* GetEquipSound() { return EquipSound;}
	FORCEINLINE int32 GetAmmoCount() const {return AmmoCount;}
	FORCEINLINE EItemType GetItemType() const {return ItemType;}
	FORCEINLINE int32 GetSlotIndex() const {return SlotIndex;}
	FORCEINLINE void SetSlotIndex(int32 Index){SlotIndex = Index;}
	FORCEINLINE void SetCharacter(AShooterCharacter* Char){Character = Char;}
	FORCEINLINE void SetInventoryFull(bool bFull){bISInventoryFull = bFull;}
	FORCEINLINE void SetPickUpSound(USoundCue* sound){PickUpSound = sound;}
	FORCEINLINE void SetEquipUpSound(USoundCue* sound){EquipSound = sound;}
	FORCEINLINE void SetItemName(FString name){ItemName = name;}
	FORCEINLINE void SetInventoryIcon(UTexture2D* Texture){ItemIcon = Texture;}
	FORCEINLINE void SetAmmoIcon(UTexture2D* Texture){AmmoIcon = Texture;}
	FORCEINLINE UMaterialInstance* GetMaterialInstance(){return MaterialInstance;}
	FORCEINLINE void SetMaterialInstance(UMaterialInstance* Instance){MaterialInstance = Instance;}
	FORCEINLINE void SetDynamicMaterialInstance(UMaterialInstanceDynamic* Instance){DynamicMaterialInstance = Instance;}
	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterialInstance(){return DynamicMaterialInstance;}
	FORCEINLINE FLinearColor GetGlowColour(){return GlowColour;}
	FORCEINLINE int32 GetMaterialIndex(){return MaterialIndex;}
	FORCEINLINE void SetMaterialIndex(int32 index){MaterialIndex = index;}
	
	void SetItemState(EItemState SetItemState);
	
	void StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound = false);

	void PlayPickupSound(bool bForcePlaySound = false);
	void PlayEquipSound(bool bForcePlaySound = false);

	virtual void EnableCustomDepth(); 
	virtual void DisableCustomDepth(); 
	virtual void InitializeCustomDepth();
	
	void EnableGlow();
	void DisableGlow();

	void StartPulseTimer();
	void ResetPulseTimer();

};
