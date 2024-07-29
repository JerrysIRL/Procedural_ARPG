// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"
#include "../Public/SkillAttributes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "MacroHelper.h"


void UBaseGameplayAbility::SetSkillAttributeClass(USkillAttributes* AttributeClass)
{
	SkillAttributes = AttributeClass;
}

void UBaseGameplayAbility::Initialize()
{
}

void UBaseGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!IsValid(SkillAttributes))
	{
		SkillAttributes = GetAvatarActorFromActorInfo()->GetComponentByClass<USkillAttributes>();
	}
}

bool UBaseGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (GetCostGameplayEffect())
		return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);

	const UAbilitySystemComponent* const ASC = ActorInfo->AbilitySystemComponent.Get();

	if (!ASC) return true;
	
	for (const auto Element : AttributeCostValues)
	{
		if (!ASC->HasAttributeSetForAttribute(Element.Key)) continue;
		if (ASC->GetNumericAttribute(Element.Key) < FMath::Abs(Element.Value)) return false;
	}
	
	return true;
}

void UBaseGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	
	if (GetCostGameplayEffect() || !ASC)
	{
		Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
		return;
	}

	

	UGameplayEffect* AttributeChangeEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AttributeChange")));
	AttributeChangeEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

	int32 Index = AttributeChangeEffect->Modifiers.Num();
	AttributeChangeEffect->Modifiers.SetNum(Index + AttributeCostValues.Num());

	for (const auto Element : AttributeCostValues)
	{
		FGameplayModifierInfo& GameplayModifierInfo = AttributeChangeEffect->Modifiers[Index];
		GameplayModifierInfo.ModifierMagnitude = FScalableFloat(Element.Value);
		GameplayModifierInfo.ModifierOp = EGameplayModOp::Additive;
		
		GameplayModifierInfo.Attribute = Element.Key;
		
		Index++;
	}
	
	ASC->ApplyGameplayEffectToSelf(AttributeChangeEffect, 1, ASC->MakeEffectContext());
}

const FGameplayTagContainer* UBaseGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void UBaseGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Calculation.Cooldown")),
		GetScaledCooldownDuration());
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

bool UBaseGameplayAbility::GetCooldownRemainingForTag(FGameplayTagContainer GECooldownTags, float& TimeRemaining,
	float& GECooldownDuration)
{
	if (AbilitySystemComponent && GECooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		GECooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(GECooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			GECooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}

	return false;
}

float UBaseGameplayAbility::GetScaledDamage() const
{
	return GetSkillAttributes().damage + Damage;
}

float UBaseGameplayAbility::GetScaledCooldownDuration() const
{
	return GetSkillAttributes().CooldownRecoverySpeed * CooldownDuration;
}

int UBaseGameplayAbility::GetProjectileAmount() const
{
	return GetSkillAttributes().ProjectileAmount;
}

void UBaseGameplayAbility::EndThisAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
