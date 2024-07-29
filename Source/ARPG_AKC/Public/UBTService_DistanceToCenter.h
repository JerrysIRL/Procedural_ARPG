// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UBTService_DistanceToCenter.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UUBTService_DistanceToCenter : public UBTService
{
	UUBTService_DistanceToCenter();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector DistanceToPlayerKey ;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector PlayerKey;
};
