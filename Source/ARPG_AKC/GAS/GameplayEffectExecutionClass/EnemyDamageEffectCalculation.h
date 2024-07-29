// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "EnemyDamageEffectCalculation.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UEnemyDamageEffectCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
private:
	static void GlobalDamageDecreases(const FGameplayTagContainer* Tags, float &Damage, UAbilitySystemComponent* ACS);

};
