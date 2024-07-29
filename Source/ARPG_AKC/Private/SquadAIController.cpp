// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadAIController.h"

#include "Squad.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASquadAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(auto OwningSquad = Cast<ASquad>(GetPawn()))
	{
		if(auto tree = OwningSquad->BehaviorTree)
		{
			UBlackboardComponent* b;
			UseBlackboard(tree->BlackboardAsset, b);
			Blackboard->SetValueAsFloat("DistanceToPlayer", TNumericLimits<float>::Max());
			Blackboard = b;
			RunBehaviorTree(tree);
		}
	}
}
