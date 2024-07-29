// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EnemyDeathManager.generated.h"


class ABaseEnemy;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, ABaseEnemy*, BaseEnemy);

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UEnemyDeathManager : public UObject
{
	GENERATED_BODY()

	
public:
	UEnemyDeathManager();
	
	FOnEnemyDied OnEnemyDied;
};
