// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletHitInterface.h"
#include "EnemyAIController.h"
#include "ShooterCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "Enemy.generated.h"

UCLASS()
class SHOOTER_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void AttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void AttackRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor*
		OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Value);
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName SectionName, float PlayRate);
	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	UFUNCTION()
	void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();
	void Die();

	void PlayHitMontage(FName SectionName, float PlayeRate = 1.0f);
	void ResetHitReact();
	UFUNCTION(BlueprintCallable)
	void StorehitNumbers(UUserWidget* Widget, FVector Location);
	UFUNCTION()
	void DestroyHitNumber(UUserWidget* Widget);
	void UpdatehitNumber();

	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void ActivateRightWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	void DestroyActor();

	void DoDamage(AShooterCharacter* Victim);
	void SpawnBloodParticle(AShooterCharacter* Victim, FName SocketName);
	void StunCharacter(AShooterCharacter* Victim);

	void ResetCanAttack();

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	FString HeadBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	float HealthBarTime;
	FTimerHandle HealthBarTimerHandle;

	FTimerHandle HitReactTimerHandle;
	bool bCanPlayHitMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MinHitReactTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHitReactTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitDestroyTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour Tree", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector Patrolpoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector Patrolpoint2;

	AEnemyAIController* EnemyAIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	USphereComponent* AgroSphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	USphereComponent* AttackSphere;
	bool bInAttackRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	bool bStunned;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float StunChance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;
	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftWeaponBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName LeftWeaponSocket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FName RightWeaponSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FTimerHandle AttackResetTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AttackResetTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bDying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float DestroActorTime;
	FTimerHandle DestroyActorTimerHandle;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* Controller) override;
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser) override;
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumbers(int32 Damage, FVector HitLocation, bool bHeadShot);

	FORCEINLINE FString GetHeadBoneName(){return HeadBoneName;}
	FORCEINLINE UBehaviorTree* GetBehaviourTree(){return BehaviorTree;}
};
