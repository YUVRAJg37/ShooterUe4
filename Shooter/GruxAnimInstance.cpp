// Fill out your copyright notice in the Description page of Project Settings.


#include "GruxAnimInstance.h"

void UGruxAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if(!Enemy)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if(Enemy)
	{
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0.0f;
		Speed = Velocity.Size();
	}
}
