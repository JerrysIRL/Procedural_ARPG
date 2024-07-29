// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BaseEnemy.h"
#include "Squad.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (auto OwningEnemy = Cast<ABaseEnemy>(GetCharacter()))
	{
		if (auto tree = OwningEnemy->GetBehaviourTree())
		{
			UBlackboardComponent* b;
			UseBlackboard(tree->BlackboardAsset, b);
			Blackboard->SetValueAsFloat("DistanceToPlayer", TNumericLimits<float>::Max());
			Blackboard->SetValueAsFloat("AttackRange", Cast<ABaseEnemy>(GetCharacter())->AttackRange);
			Blackboard->SetValueAsBool("CanBeStaggered", OwningEnemy->CanBeStaggered);
			Blackboard = b;
			RunBehaviorTree(tree);
		}
	}
}
