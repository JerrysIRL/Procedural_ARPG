// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "ChainLightningAbility.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class ARPG_AKC_API UChainLightningAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

	UChainLightningAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void SpawnLightningNiagara(const FVector& StartLocation, const FVector& EndLocation, const FVector& ImpactNormal) const;
	bool FindClosestActor(const FVector& Location, AActor*& ClosestActor, const TArray<AActor*>& HitActors) const;

	void ChainLightning(FVector& StartLocation, FVector& EndLocation, FVector ImpactNormal, int32& Amount, TArray<AActor*>& HitActors) const;

	static FVector GetRandomUnitVectorInCone(const FVector& ConeDir, float MinAngle, float MaxAngle);
	
private:

	
	
	UPROPERTY(EditAnywhere, Category = "ChainLightning")
	UNiagaraSystem* N_ChainLightning;

	UPROPERTY(EditAnywhere, Category = "ChainLightning")
	TSubclassOf<UGameplayEffect> GameplayEffect;
	
	UPROPERTY(EditAnywhere, Category = "ChainLightning")
	float MaxRange = 500.f;

	UPROPERTY(EditAnywhere, Category = "ChainLightning")
	int32 MaxChain = 4;

	UPROPERTY(EditAnywhere, Category = "ChainLightning")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQueries;

	UPROPERTY(EditAnywhere, Category = "ChainLightning")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
};
