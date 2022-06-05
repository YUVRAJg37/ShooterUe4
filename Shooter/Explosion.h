// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHitInterface.h"
#include "Components/SphereComponent.h"
#include "Explosion.generated.h"

UCLASS()
class SHOOTER_API AExplosion : public AActor, public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ExplodeParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ExplodeSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	USphereComponent* OverlapSphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BulletHit_Implementation(FHitResult HitResul, AActor* Shooter, AController* Instigator) override;

};
