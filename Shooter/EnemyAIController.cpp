// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("Behaviour Tree Component");
	check(BehaviorTreeComponent);

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("Black Board Component");
	check(BlackboardComponent);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(InPawn)
	{
		AEnemy* Enemy = Cast<AEnemy>(InPawn);
		if(Enemy->GetBehaviourTree() && Enemy)
		{
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviourTree()->GetBlackboardAsset()));
		}
	}
}
