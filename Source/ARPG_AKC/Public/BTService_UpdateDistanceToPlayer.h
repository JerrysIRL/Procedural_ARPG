// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdateDistanceToPlayer.generated.h"


UCLASS()
class ARPG_AKC_API UBTService_UpdateDistanceToPlayer : public UBTService
{
	GENERATED_BODY()
	
	UBTService_UpdateDistanceToPlayer();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector SelfActorKey;
	
	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector DistanceToPlayerKey ;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector PlayerKey;
};
