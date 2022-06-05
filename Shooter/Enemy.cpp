// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "ShooterCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemy::AEnemy() :
MaxHealth(100.0f),
HealthBarTime(4.0f),
bCanPlayHitMontage(true),
MinHitReactTime(0.5f),
MaxHitReactTime(3.0f),
HitDestroyTime(1.5f),
bStunned(false),
StunChance(0.5f),
bInAttackRange(false),
AttackLFast(FName("AttackLAFast")),
AttackRFast(FName("AttackRAFast")),
AttackL(FName("AttackLA")),
AttackR(FName("AttackRA")),
BaseDamage(10.0f),
LeftWeaponSocket(FName("FX_Trail_L_01")),
RightWeaponSocket(FName("FX_Trail_R_01")),
AttackResetTime(1.0f),
bCanAttack(true),
bDying(false),
DestroActorTime(5.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>("Sphere Component");
	AgroSphere->SetupAttachment(GetRootComponent());

	AttackSphere = CreateDefaultSubobject<USphereComponent>("Attack Sphere");
	AttackSphere->SetupAttachment(GetRootComponent());

	LeftWeaponBox = CreateDefaultSubobject<UBoxComponent>("Left Weapon Box");
	LeftWeaponBox->SetupAttachment(GetMesh(), FName("LeftWeaponSocket"));
	RightWeaponBox = CreateDefaultSubobject<UBoxComponent>("Right Weapon Box");
	RightWeaponBox->SetupAttachment(GetMesh(), FName("RightWeaponSocket"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyAIController = Cast<AEnemyAIController>(GetController());

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	LeftWeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponBox->SetCollisionObjectType(ECC_WorldDynamic);
	LeftWeaponBox->SetCollisionResponseToChannels(ECR_Ignore);
	LeftWeaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RightWeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponBox->SetCollisionObjectType(ECC_WorldDynamic);
	RightWeaponBox->SetCollisionResponseToChannels(ECR_Ignore);
	RightWeaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	FVector WorldPatrolPosition = UKismetMathLibrary::TransformLocation(GetActorTransform(), Patrolpoint);
	FVector WorldPatrolPosition2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), Patrolpoint2);

	if(EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPosition);
		EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPosition2);
		EnemyAIController->RunBehaviorTree(BehaviorTree);
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), false);
	}

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackRangeOverlap);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackRangeEnd);

	LeftWeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AEnemy::OnRightWeaponOverlap);

	Health = MaxHealth;
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimerHandle);
	GetWorldTimerManager().SetTimer(HealthBarTimerHandle, this, &AEnemy::HideHealthBar, HealthBarTime);
}

void AEnemy::Die()
{
	if(bDying) return;
	bDying = true;
	
	HideHealthBar();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
	if(EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
		EnemyAIController->StopMovement();
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatehitNumber();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ControllerInstigator)
{
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitResult.Location, FRotator{0.0f}, true);
	}

	if(bDying) return;
	
	ShowHealthBar();

	const float Stunned = FMath::RandRange(0.0f, 1.0f);
	if(Stunned<=StunChance)
	{
		PlayHitMontage(FName("PlayHitMontage"));
		SetStunned(true);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	AShooterCharacter* Character = Cast<AShooterCharacter>(DamageCauser);
	
	if(EnemyAIController && Character)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), Character);
	}
	if(Health - DamageAmount < 0)
	{
		Health = 0;
		Die();
	}
	else
	{
		Health-=DamageAmount;
	}

	if(bDying) return DamageAmount;
	
	ShowHealthBar();

	const float Stunned = FMath::RandRange(0.0f, 1.0f);
	if(Stunned<=StunChance)
	{
		PlayHitMontage(FName("PlayHitMontage"));
		SetStunned(true);
	}
	
	return DamageAmount;
}
void AEnemy::PlayHitMontage(FName SectionName, float PlayeRate)
{
	if(bCanPlayHitMontage)
	{
		bCanPlayHitMontage = false;
		if(HitAnimMontage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(HitAnimMontage, PlayeRate);
			AnimInstance->Montage_JumpToSection(SectionName);
			const float HitReactTime = FMath::RandRange(MinHitReactTime, MaxHitReactTime);
			GetWorldTimerManager().SetTimer(HitReactTimerHandle, this, &AEnemy::ResetHitReact, HitReactTime);
		}
	}
}

void AEnemy::ResetHitReact()
{
	bCanPlayHitMontage = true;
}

void AEnemy::StorehitNumbers(UUserWidget* Widget, FVector Location)
{
	HitNumbers.Add(Widget, Location);
	FTimerHandle HitTimerHandle;
	FTimerDelegate HitTimerDelegate;
	HitTimerDelegate.BindUFunction(this, FName("DestroyHitNumber"), Widget);
	GetWorldTimerManager().SetTimer(HitTimerHandle, HitTimerDelegate, HitDestroyTime, false);
}

void AEnemy::DestroyHitNumber(UUserWidget* Widget)
{
	HitNumbers.Remove(Widget);
	Widget->RemoveFromParent();
}

void AEnemy::UpdatehitNumber()
{
	for(auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber{HitPair.Key};
		const FVector HitLocation{HitPair.Value};
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), HitLocation, ScreenPosition);
		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if(ShooterCharacter)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), ShooterCharacter);
	}
}

void AEnemy::SetStunned(bool Value)
{
	bStunned = Value;
	if(EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bStunned);
	}
}

void AEnemy::AttackRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr) return;
	auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if(ShooterCharacter)
	{
		bInAttackRange = true;
		if(EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("InAttackRange"), true);
		}
	}
}

void AEnemy::AttackRangeEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor == nullptr) return;
	auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if(ShooterCharacter)
	{
		bInAttackRange = true;
		if(EnemyAIController)
		{
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("InAttackRange"), false);
		}
	}
}

void AEnemy::PlayAttackMontage(FName SectionName, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(SectionName);
	}

	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackResetTimerHandle, this, &AEnemy::ResetCanAttack, AttackResetTime);
	if(EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
	}
}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section = FMath::RandRange(1, 4);
	switch (Section)
	{
	case 1 :
		{
			SectionName = AttackLFast;
			break;
		}
	case 2:
		{
			SectionName = AttackRFast;
			break;
		}
	case 3:
		{
			SectionName = AttackL;
			break;
		}
	case 4 :
		{
			SectionName = AttackR;
			break;
		}
	}

	return SectionName;
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if(Character)
	{
		DoDamage(Character);
		SpawnBloodParticle(Character, LeftWeaponSocket);
		StunCharacter(Character);
	}
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if(Character)
	{
		DoDamage(Character);
		SpawnBloodParticle(Character, RightWeaponSocket);
		StunCharacter(Character);
	}
}

void AEnemy::ActivateLeftWeaponCollision()
{
	LeftWeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeaponCollision()
{
	LeftWeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeaponCollision()
{
	RightWeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeaponCollision()
{
	RightWeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DestroyActorTimerHandle, this, &AEnemy::DestroyActor, DestroActorTime);
}

void AEnemy::DestroyActor()
{
	Destroy();
}

void AEnemy::DoDamage(AShooterCharacter* Victim)
{
	if(Victim == nullptr) return;
	
	if(Victim)
	{
		UGameplayStatics::ApplyDamage(Victim, BaseDamage, EnemyAIController, this,
			UDamageType::StaticClass());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Victim->GetMeleeSoundCue(), Victim->GetActorLocation());
	}
}

void AEnemy::SpawnBloodParticle(AShooterCharacter* Victim, FName SocketName)
{
	if(Victim)
	{
		const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName(SocketName);
		if(TipSocket)
		{
			const FTransform SocketTrans = TipSocket->GetSocketTransform(GetMesh());
			if(Victim->GetBloodParticle())
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Victim->GetBloodParticle(), SocketTrans);
			}
		}
	}
}

void AEnemy::StunCharacter(AShooterCharacter* Victim)
{
	if(Victim)
	{
		const float StunRand = FMath::RandRange(0 , 1);
		if(StunRand <= Victim->GetStunChance())
		{
			Victim->Stun();
		}
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if(EnemyAIController)
	{
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}
}
