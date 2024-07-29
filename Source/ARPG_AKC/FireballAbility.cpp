// Fill out your copyright notice in the Description page of Project Settings.


#include "FireballAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTask.h"
#include "ARPG_AKCCharacter.h"
#include "MacroHelper.h"
#include "ProjectileBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Public\SkillAttributes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UFireballAbility::UFireballAbility()
{
	FGameplayTag FireballTag = FGameplayTag::RequestGameplayTag(FName("Action.Fireball"));
	AbilityTags.AddTag(FireballTag);

	Name = "Fireball";
}

void UFireballAbility::SpawnFireball()
{
	const FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetAvatarActorFromActorInfo();
	SpawnParameters.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileSubClass, SpawnLocation, 
	SpawnRotation, SpawnParameters);
	
	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, 1);
	EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Calculation.Damage"),GetScaledDamage());
	
	ApplyAbilityTagsToGameplayEffectSpec(*EffectSpecHandle.Data.Get(), GetCurrentAbilitySpec());

	Projectile->SpecHandle = EffectSpecHandle;
}

void UFireballAbility::Initialize()
{
	Super::Initialize();
}

void UFireballAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!IsSkillAttributeValid())
	{
		DEBUG_PRINT("Skill Attributes is null")
		EndThisAbility();
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndThisAbility();
	}

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, "MeteorStrike", Montage, 2.f, NAME_None, false, 1.f, 0.2f);
	Task->Activate();
	
	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false);

	AActor* Actor = GetAvatarActorFromActorInfo();

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	FVector WorldLocation, WorldDirection;
	
	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	SpawnLocation = Actor->GetActorForwardVector() * 100 + Actor->GetActorLocation();

	const FVector PlaneOrigin(0.0f, 0.0f, SpawnLocation.Z);


	TargetLocation = FMath::LinePlaneIntersection(
			WorldLocation,
			WorldLocation + WorldDirection,
			PlaneOrigin,
			FVector::UpVector);
	
	Actor->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), TargetLocation));
	SpawnLocation = Actor->GetActorForwardVector() * 100 + Actor->GetActorLocation();
	

	for (int i = 0; i < GetProjectileAmount(); ++i)
	{
		UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, .1f + i *.1f);
		DelayTask->OnFinish.AddDynamic(this, &UFireballAbility::SpawnFireball);

		if (i == GetProjectileAmount() - 1)
		{
			DelayTask->OnFinish.AddDynamic(this, &UBaseGameplayAbility::EndThisAbility);
		}
		
		DelayTask->Activate();
	}
}