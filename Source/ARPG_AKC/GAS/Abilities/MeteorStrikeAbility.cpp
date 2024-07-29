// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPG_AKC/GAS/Abilities/MeteorStrikeAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UMeteorStrikeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, "MeteorStrike", Montage, 1.f);
	
	Task->OnBlendOut.AddUniqueDynamic(this, &UMeteorStrikeAbility::OnAnimCompleted);
	Task->OnCancelled.AddUniqueDynamic(this, &UMeteorStrikeAbility::EndThisAbility);
	Task->OnInterrupted.AddUniqueDynamic(this, &UMeteorStrikeAbility::EndThisAbility);
	Task->OnCompleted.AddUniqueDynamic(this, &UMeteorStrikeAbility::EndThisAbility);
	Task->ReadyForActivation();
}

void UMeteorStrikeAbility::OnAnimCompleted()
{
	if (auto Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		auto Owner = Cast<ABaseEnemy>(GetAvatarActorFromActorInfo());
		auto OutSpec = Owner->GetDamageSpec(Damage, FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Damage.Element.Fire")));
		
		SpawnMeteor(GetFuturePosition(Player), OutSpec);
	}
}

FVector UMeteorStrikeAbility::GetFuturePosition(const ACharacter* Player) const
{
	FVector PlayerVelocity = Player->GetVelocity() * DelayTime;

	const float RandX = FMath::RandRange(-RandomPositionDeviation, RandomPositionDeviation);
	const float RandY = FMath::RandRange(-RandomPositionDeviation, RandomPositionDeviation);

	float PlayerHalfHeight = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector RandomOffset = FVector(RandX, RandY, -PlayerHalfHeight);
	return Player->GetActorLocation() + PlayerVelocity + RandomOffset;
}
