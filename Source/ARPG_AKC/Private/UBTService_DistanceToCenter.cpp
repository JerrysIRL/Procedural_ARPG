// Fill out your copyright notice in the Description page of Project Settings.


#include "UBTService_DistanceToCenter.h"

#include "AIController.h"
#include "Squad.h"
#include "BehaviorTree/BlackboardComponent.h"

UUBTService_DistanceToCenter::UUBTService_DistanceToCenter()
{
	NodeName = "UpdateSquadDistanceToPlayer";
}

void UUBTService_DistanceToCenter::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ASquad* Squad = Cast<ASquad>(OwnerComp.GetAIOwner()->GetPawn());

	if(!Squad)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTService_DistanceToCenter::TickNode: Squad is nullptr"));
	}
	AActor* PlayerActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerKey.SelectedKeyName));

	if(Squad && PlayerActor)
	{
		const float Dist = FVector::Dist(Squad->GetActorLocation(), PlayerActor->GetActorLocation());
		
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceToPlayerKey.SelectedKeyName, Dist);
	}
}
 