// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BaseGameplayAbility.h"
#include "ARPG_AKC/ARPG_AKCCharacter.h"
#include "BaseEnemy.generated.h"

class UEnemyAttributeSet;
class UAbilitySystemComponent;
class UBehaviorTree;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDead, ABaseEnemy*, Sender);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyHit);

UCLASS()
class ARPG_AKC_API ABaseEnemy : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	ABaseEnemy();

	UPROPERTY(EditDefaultsOnly, Category=AI, meta=(AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY()
	AARPG_AKCCharacter* Player;

	UFUNCTION()
	void OnDead(ABaseEnemy* BaseEnemy);

public:
	UPROPERTY(BlueprintCallable)
	FOnEnemyDead OnPlayerDead;

	UPROPERTY(BlueprintCallable)
	FOnEnemyHit OnPlayerHit;

	FORCEINLINE float GetExperience() const { return Experience; }

	UPROPERTY(EditDefaultsOnly, Category=AI, meta=(AllowPrivateAccess = "true"))
	UBehaviorTree* AttackBehaviorTree = nullptr;

	UFUNCTION(BlueprintCallable)
	FGameplayEffectSpecHandle GetDamageSpec(float Damage, const FGameplayTagContainer TagsToAdd);

	UPROPERTY(EditDefaultsOnly)
	float AttackRange = 100;

	UBehaviorTree* GetBehaviourTree() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	bool CanBeStaggered = false;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	float Experience = 25.f;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	FGameplayTag DamageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TSubclassOf<UGameplayEffect> DamageEffect;
	/* Starting Abilities */
	UPROPERTY(BlueprintReadOnly, Category="Abilities")
	const UEnemyAttributeSet* Attributes;

	UFUNCTION(BlueprintImplementableEvent)
	void HandleDeath(ABaseEnemy* Enemy);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attack")
	bool IsWithinAttackRange();

	UPROPERTY(EditDefaultsOnly, Category="Attack")
	float HitDistanceDeviation = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Attack")
	float HitAngle = 30.f;

	/* Animation montage to play when attacking */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	UAnimMontage* StaggeredAnimation;

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(FGameplayEffectSpecHandle SpecHandle);

	UFUNCTION(BlueprintCallable)
	void StopLogic();

	UFUNCTION(BlueprintCallable)
	void Ragdoll();

	UFUNCTION()
	void OnHit();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=AI)
	float BaseDamage = 5.f;

	virtual void BeginPlay() override;
};
