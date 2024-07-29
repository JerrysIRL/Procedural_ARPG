// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_IsFacing.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UBTDecorator_IsFacing : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

public:
	UBTDecorator_IsFacing();

private:
	/* Field of view of the EnemyAI */
	UPROPERTY()
	float FieldOfView = 90;
	
public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
