// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageEffectCalculation.h"

#include "EnemyAttributeSet.h"
#include "ARPG_AKC/PlayerAttributeSet.h"

struct FEnemyDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	FEnemyDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnemyAttributeSet, MaxHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnemyAttributeSet, Health, Target, false);
	}
};

static FEnemyDamageStatics& GetDamageEnemyStatics()
{
	static FEnemyDamageStatics DamageStatistics;
	return DamageStatistics;
}

UDamageEffectCalculation::UDamageEffectCalculation()
{
	RelevantAttributesToCapture.Add(GetDamageEnemyStatics().HealthDef);
	RelevantAttributesToCapture.Add(GetDamageEnemyStatics().MaxHealthDef);
}

void UDamageEffectCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                      FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetABSC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetABSC ? TargetABSC->GetAvatarActor() : nullptr;

	UAbilitySystemComponent* SourceABSC = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceABSC ? SourceABSC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float Damage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Calculation.Damage"));
	Damage = FMath::Abs(Damage);
	
	GlobalDamageIncreases(SourceTags, Damage, SourceABSC);
	
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(GetDamageEnemyStatics().HealthProperty, EGameplayModOp::Additive, -Damage)
		);
}

void UDamageEffectCalculation::GlobalDamageIncreases(const FGameplayTagContainer* Tags, float& Damage, UAbilitySystemComponent* ACS)
{
	const UPlayerAttributeSet* PlayerAttributeSet = Cast<UPlayerAttributeSet>(ACS->GetAttributeSet(UPlayerAttributeSet::StaticClass()));
	if (!IsValid(PlayerAttributeSet)) return;
	
	float Multiplier = 1;
	
	
	for (auto Tag : Tags->GetGameplayTagArray())
	{
		if (Tag == FGameplayTag::RequestGameplayTag("Damage.Element.Fire"))
		{
			Multiplier += PlayerAttributeSet->GetPercentageFireDamage() / 100.f;

			Damage += PlayerAttributeSet->GetFlatFireDamage();
		}
		
		if (Tag == FGameplayTag::RequestGameplayTag("Damage.Element.Lightning"))
		{
			Multiplier += PlayerAttributeSet->GetPercentageLightningDamage() / 100.f;

			Damage += PlayerAttributeSet->GetFlatLightningDamage();
		}

		if (Tag == FGameplayTag::RequestGameplayTag("Damage.Element.Physical"))
		{
			Multiplier += PlayerAttributeSet->GetPercentagePhysicalDamage();

			Damage += PlayerAttributeSet->GetFlatPhysicalDamage();
		}

		
		if (Tag == FGameplayTag::RequestGameplayTag("Damage.Element.Spell"))
		{
			Multiplier += PlayerAttributeSet->GetPercentageSpellDamage();

			Damage += PlayerAttributeSet->GetFlatSpellDamage();
		}
	}

	Damage *= Multiplier;
}
