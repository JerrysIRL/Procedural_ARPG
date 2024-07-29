// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAttributeSet.h"

#include "GameplayEffectExtension.h"

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxMana());
	}

	if (Attribute == GetExperienceAttribute())
	{
		NewValue = FMath::Abs(NewValue);


		const int XPAmountNeeded = XPNeeded(GetPlayerLevel());
		while (NewValue >= XPAmountNeeded)
		{
			const float excessEXP = NewValue - XPAmountNeeded;
			PlayerLevel.SetCurrentValue(PlayerLevel.GetCurrentValue() + 1);
			NewValue = excessEXP;
			PlayerLevelUpDelegate.Broadcast();
		}
	}
}

int UPlayerAttributeSet::XPNeeded(int Level)
{
	const double Base_XP = 1000;
	
	// Define XP increase factor per level
	const double XP_Increase_Factor = 1.1;

	// Calculate total XP needed to level up
	return Base_XP * pow(XP_Increase_Factor, Level + 1);
}

void UPlayerAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	AttributeChanged.Broadcast(Attribute.AttributeName, NewValue);
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp<float>(GetMana(), 0, GetMaxMana()));
	}
}
