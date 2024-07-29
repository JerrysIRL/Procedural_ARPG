// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "Curves/CurveFloat.h"
#include "DashAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UDashAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

	UDashAbility();
	
public:
	UFUNCTION()
	void Lerp(const FVector& Start, const FVector& End, const float Duration);
protected:
	void StartDash(FVector& Direction);
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	
private:
	UPROPERTY(EditAnywhere)
	class UCurveFloat* CurveFloat;
	
	UPROPERTY(EditAnywhere)
	float Speed = 200.f;

	UPROPERTY(EditAnywhere)
	float Length = 600.f;

	float StartTime;
	
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ETraceTypeQuery> CollisionChannel;

	
	
};
