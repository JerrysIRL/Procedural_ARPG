// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UCrowdFollowingComponent;

/**
 * 
 */
UCLASS()
class ARPG_AKC_API AEnemyAIController : public AAIController
{
	// AEnemyAIController(const FObjectInitializer& ObjectInitializer);
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;
	
};
