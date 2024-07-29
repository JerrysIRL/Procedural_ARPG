// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageEffectCalculation.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UDamageEffectCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UDamageEffectCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	static void GlobalDamageIncreases(const FGameplayTagContainer* Tags, float &Damage, UAbilitySystemComponent* ACS);
	
};
