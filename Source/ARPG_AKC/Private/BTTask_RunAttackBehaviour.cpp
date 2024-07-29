// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RunAttackBehaviour.h"

#include "AIController.h"
#include "BaseEnemy.h"
#include "VisualLogger/VisualLogger.h"
#include "BehaviorTree/BlackboardData.h"


UBTTask_RunAttackBehaviour::UBTTask_RunAttackBehaviour()
{
	NodeName = "RunAttackBehaviour";
}

EBTNodeResult::Type UBTTask_RunAttackBehaviour::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BehaviorAsset = Cast<ABaseEnemy>(OwnerComp.GetAIOwner()->GetCharacter())->AttackBehaviorTree;
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
