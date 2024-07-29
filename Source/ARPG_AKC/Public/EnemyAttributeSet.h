// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "ARPG_AKC/AttributeHelpers.h"
#include "EnemyAttributeSet.generated.h"


UCLASS()
class ARPG_AKC_API UEnemyAttributeSet : public UAttributeSet
{

	GENERATED_BODY()
public:
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	FGameplayAttributeData Health;
	
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Health);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character")
	FGameplayAttributeData MaxHealth;

	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, MaxHealth);
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};
