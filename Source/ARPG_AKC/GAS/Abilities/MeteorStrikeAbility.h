// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "MeteorStrikeAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UMeteorStrikeAbility : public UBaseGameplayAbility
{
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnMeteor(FVector Position,FGameplayEffectSpecHandle SpecHandle);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DelayTime = 1.f;
	
	UPROPERTY(EditDefaultsOnly)
	float RandomPositionDeviation = 40.f;

	UFUNCTION()
	void OnAnimCompleted();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	FVector GetFuturePosition(const ACharacter* Player) const;

private:
	GENERATED_BODY()
};
