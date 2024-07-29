// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RunBehavior.h"
#include "BTTask_RunAttackBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UBTTask_RunAttackBehaviour : public UBTTask_RunBehavior
{
	GENERATED_BODY()
	UBTTask_RunAttackBehaviour();

public:
	virtual  EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
