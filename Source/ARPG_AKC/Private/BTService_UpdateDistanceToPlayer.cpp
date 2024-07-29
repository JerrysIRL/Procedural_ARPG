// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateDistanceToPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_UpdateDistanceToPlayer::UBTService_UpdateDistanceToPlayer()
{
	NodeName = "UpdateDistanceToPlayer";
}

void UBTService_UpdateDistanceToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AActor* SelfActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()
		->GetValueAsObject(SelfActorKey.SelectedKeyName));
	AActor* PlayerActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()
		->GetValueAsObject(PlayerKey.SelectedKeyName));

	if(SelfActor && PlayerActor)
	{
		const float Dist = FVector::Dist(SelfActor->GetActorLocation(), PlayerActor->GetActorLocation());
		
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceToPlayerKey.SelectedKeyName, Dist);
	}
}
