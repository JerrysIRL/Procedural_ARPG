// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeHelpers.h"
#include "PlayerAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLevelUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, FString, AttributeDisplayName, float, NewValue );

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData PercentageFireDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, PercentageFireDamage);
	
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData FlatFireDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FlatFireDamage)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData PercentageLightningDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, PercentageLightningDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData FlatLightningDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FlatLightningDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData PercentageSpellDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, PercentageSpellDamage);
	
	
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData FlatSpellDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FlatSpellDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData PercentagePhysicalDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, PercentagePhysicalDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData FlatPhysicalDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FlatPhysicalDamage);
	
	UPROPERTY(BlueprintReadOnly, Category = "Resistance")
	FGameplayAttributeData FlatPhysicalResistance;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FlatPhysicalResistance);

	UPROPERTY(BlueprintReadOnly, Category = "Resistance")
	FGameplayAttributeData FlatFireResistance;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FlatFireResistance);
	

	UPROPERTY(BlueprintReadOnly, Category = "Resistance")
	FGameplayAttributeData FlatLightningResistance; 
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FlatLightningResistance);
	
	
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, Category = "Experience")
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Experience);

	UPROPERTY(BlueprintReadOnly, Category = "Experience")
	FGameplayAttributeData PlayerLevel;
	ATTRIBUTE_GETTERS(UPlayerAttributeSet, PlayerLevel);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerLevelUp PlayerLevelUpDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged AttributeChanged;
private:
	static int XPNeeded(int Level);

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
