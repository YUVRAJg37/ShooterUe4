// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:

	AEnemyAIController();
	void OnPossess(APawn* InPawn) override;

private:

	UPROPERTY(BlueprintReadWrite, Category = "AI Behaviour", meta = (AllowPrivateAccess = "true"))
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(BlueprintReadWrite, Category = "AI Behaviour", meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComponent;

	
public:

	FORCEINLINE UBehaviorTreeComponent* GetBehaviourTreeComponent(){return BehaviorTreeComponent;}
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent(){return BlackboardComponent;}
};
