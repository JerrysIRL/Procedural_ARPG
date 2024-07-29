// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomLocation.h"

#include "EnemyAIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	NodeName = "Find Random Location";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto PlayerPawn = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (auto navMesh = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			FNavLocation result;
			if (navMesh->GetRandomPointInNavigableRadius(PlayerPawn->GetActorLocation(),
			OwnerComp.GetBlackboardComponent()->GetValueAsFloat("AttackRange") - FMath::RandRange(0.0f, AcceptanceDeviation), result))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), result);

				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}
