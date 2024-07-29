// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillAttributes.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameplayAbility.generated.h"

struct FSkillAttributesStruct;
class UAbilitySystemComponent;
class USkillAttributes;
class UTexture2D;
/**
 * 
 */
UCLASS()
class ARPG_AKC_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	/*The name of the ability*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name = "Ability Name";


	void SetSkillAttributeClass(USkillAttributes* AttributeClass);
	virtual void Initialize();

	UPROPERTY(EditAnywhere)
	UDataTable* SkillTreeDataTable;

	int SkillTreeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image;

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* Montage = nullptr;

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Damage = 20;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FGameplayAttribute, float> AttributeCostValues;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	float CooldownDuration;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FGameplayTagContainer CooldownTags;

	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	UFUNCTION(BlueprintCallable)
	bool GetCooldownRemainingForTag(FGameplayTagContainer GECooldownTags, float & TimeRemaining, float & GECooldownDuration);

	FSkillAttributesStruct& GetSkillAttributes() const
	{
		return SkillAttributes->SkillAttributes[SkillTreeIndex];
	}

	bool IsSkillAttributeValid() const
	{
		return IsValid(SkillAttributes);
	}

	virtual float GetScaledDamage() const;
	virtual float GetScaledCooldownDuration() const;
	virtual int GetProjectileAmount() const;
	
public:
	UFUNCTION()
	virtual void EndThisAbility();
	
private:
	UPROPERTY()
	USkillAttributes* SkillAttributes;
	
};
