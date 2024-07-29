// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDamageEffectCalculation.h"

#include "EnemyAttributeSet.h"
#include "ARPG_AKC/PlayerAttributeSet.h"

struct FDamageStatistics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	FDamageStatistics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Armor, Target, false);
	}
};

static FDamageStatistics& GetDamageStats()
{
	static FDamageStatistics DamageStatistics;
	return DamageStatistics;
}

void UEnemyDamageEffectCalculation::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;


	float Damage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Calculation.Damage"));
	Damage = FMath::Abs(Damage);

	float Armor;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStats().ArmorDef, EvaluateParameters, Armor);

	GlobalDamageDecreases(SourceTags, Damage, TargetASC);

	float DamageMitigation = Armor / (Armor + 10*Damage);
	DamageMitigation = 1 - DamageMitigation;

	Damage *= DamageMitigation;

	OutExecutionOutput.AddOutputModifier(
	FGameplayModifierEvaluatedData(GetDamageStats().HealthProperty, EGameplayModOp::Additive, -Damage)
	);
}

void UEnemyDamageEffectCalculation::GlobalDamageDecreases(const FGameplayTagContainer* Tags, float& Damage,
	UAbilitySystemComponent* ACS)
{
	const UPlayerAttributeSet* PlayerAttributeSet = Cast<UPlayerAttributeSet>(ACS->GetAttributeSet(UPlayerAttributeSet::StaticClass()));
	if (!IsValid(PlayerAttributeSet)) return;
	float Multiplier = 1;

	for (auto Tag : Tags->GetGameplayTagArray())
	{
		if (Tag == FGameplayTag::RequestGameplayTag("Damage.Element.Fire"))
		{
			Multiplier -= PlayerAttributeSet->GetFlatFireResistance() / 100.f;

		}
		
		if (Tag == FGameplayTag::RequestGameplayTag("Damage.Element.Lightning"))
		{
			Multiplier -= PlayerAttributeSet->GetFlatFireResistance() / 100.f;
		}

		if (Tag == FGameplayTag::RequestGameplayTag("Damage.Element.Physical"))
		{
			Multiplier += PlayerAttributeSet->GetFlatPhysicalResistance() / 100.f;
		}
	}

	Multiplier = FMath::Clamp(Multiplier, .25f, 1.f);

	Damage *= Multiplier;
}
