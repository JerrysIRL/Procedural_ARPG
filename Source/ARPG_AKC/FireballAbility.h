// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "FireballAbility.generated.h"

class AProjectileBase;
/**
 * 
 */
UCLASS()
class ARPG_AKC_API UFireballAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

	UFireballAbility();
	

protected:
	UFUNCTION()
	void SpawnFireball();

	virtual void Initialize() override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fireball")
	TSubclassOf<AProjectileBase> ProjectileSubClass;
	
	FVector SpawnLocation;
	FVector TargetLocation;

	UPROPERTY(EditAnywhere, Category = "Fireball")
	TSubclassOf<UGameplayEffect> GameplayEffect;
	
};