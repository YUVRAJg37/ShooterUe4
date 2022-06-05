// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosion.h"

#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AExplosion::AExplosion() :
BaseDamage(90.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	RootComponent = Mesh;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere");
	OverlapSphere->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosion::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* Controller)
{
	if(ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}
	if(ExplodeParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticle, HitResult.Location, FRotator{0.0f}, true);
	}

	TArray<AActor*> OverlappingActor;
	GetOverlappingActors(OverlappingActor, ACharacter::StaticClass());

	for(auto Actor : OverlappingActor)
	{
		UGameplayStatics::ApplyDamage(Actor, BaseDamage, Controller, Shooter, UDamageType::StaticClass());
	}
	
	Destroy();
}

