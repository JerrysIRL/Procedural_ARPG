// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAttributeSet.h"
#include "BaseEnemy.h"

void UEnemyAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if(Attribute == GetHealthAttribute())
	{
		// Handle death
		auto Owner = Cast<ABaseEnemy>(GetOwningActor());
		if(!Owner)
			return;
			
		Owner->OnPlayerHit.Broadcast();
		if(NewValue <= 0.f)
		{
			 Owner->OnPlayerDead.Broadcast(Owner);
		}
	}
}

void UEnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}
