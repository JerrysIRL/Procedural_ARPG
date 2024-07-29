// Fill out your copyright notice in the Description page of Project Settings.


#include "ChainLightningAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BaseEnemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UChainLightningAbility::UChainLightningAbility()
{
	Name = "Chain Lightning";
}

void UChainLightningAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	//Animation
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, "ChainLightning", Montage, 2.5f, NAME_None, false, 1.f, 0.2f);
	Task->Activate();
	
	AActor* Actor;
	FVector StartLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	TArray<AActor*> HitActors = {GetAvatarActorFromActorInfo()};

	if (!FindClosestActor(StartLocation, Actor, HitActors))
	{
		
		FHitResult HitResult;
		
		FVector RandomDirection = GetRandomUnitVectorInCone(FVector::DownVector, 10.f, 22.5f);

		FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
		CQP.AddIgnoredActor(GetAvatarActorFromActorInfo());
		
		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			StartLocation + RandomDirection * MaxRange,
			ECC_Visibility
			);

		FVector EndLocation = HitResult.ImpactPoint;
		FVector ImpactNormal = HitResult.ImpactNormal;
		
		SpawnLightningNiagara(StartLocation, EndLocation, ImpactNormal);
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}

	if (!Actor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}

	FVector EndLocation = Actor->GetActorLocation();
	FVector ImpactNormal = EndLocation - Actor->GetActorLocation();
	ImpactNormal.Normalize();

	int32 Amount = MaxChain * GetProjectileAmount();

	auto Owner = GetAvatarActorFromActorInfo();
	Owner->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), EndLocation));
	ChainLightning(StartLocation, EndLocation, ImpactNormal, Amount, HitActors);
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UChainLightningAbility::SpawnLightningNiagara(const FVector& StartLocation, const FVector& EndLocation, const FVector& ImpactNormal) 
const
{
	FFXSystemSpawnParameters SpawnParameters;
	SpawnParameters.Location = FVector::ZeroVector;
	SpawnParameters.Scale = FVector::OneVector;
	SpawnParameters.bAutoDestroy = true;
	SpawnParameters.bAutoActivate = false;
	SpawnParameters.SystemTemplate = N_ChainLightning;
	SpawnParameters.WorldContextObject = GetWorld();
	
	
	UNiagaraComponent* ChainNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocationWithParams(SpawnParameters);

	ChainNiagara->SetVectorParameter("BeamStartPoint", StartLocation);
	ChainNiagara->SetVectorParameter("BeamEndPoint", EndLocation);
	ChainNiagara->SetVectorParameter("ImpactNormal", ImpactNormal);

	ChainNiagara->Activate();
}

bool UChainLightningAbility::FindClosestActor(const FVector& Location, AActor*& ClosestActor, const TArray<AActor*>& HitActors) const
{
	TArray<AActor*> Actors;
	const ETraceTypeQuery QueryChannel = UEngineTypes::ConvertToTraceType(CollisionChannel);
	TArray<FHitResult> HitResults;
	bool SphereCast = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Location,
		Location,
		MaxRange,
		QueryChannel,
		false,
		HitActors,
		EDrawDebugTrace::None,
		HitResults,
		true
	);

	if (HitResults.IsEmpty()) return false;

	float ClosestDistance = MaxRange;

	for (FHitResult hit : HitResults)
	{
		auto Actor = hit.GetActor();
		if(HitActors.Contains(Actor) || !IsValid(Actor))
			continue;
			
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *Actor->GetName());
		const float Distance = FVector::Distance(Location, Actor->GetActorLocation()); 
		if (Distance > ClosestDistance) continue;

		ClosestDistance = Distance;
		ClosestActor = Actor;
	}
	
	return ClosestActor != nullptr;
}

void UChainLightningAbility::ChainLightning(FVector& StartLocation, FVector& EndLocation, FVector ImpactNormal, int32& Amount, TArray<AActor*>& HitActors) const
{
	if (Amount <= 0) return;
	Amount--;
	
	SpawnLightningNiagara(StartLocation, EndLocation, ImpactNormal);

	AActor* ClosestActor;
	if (!FindClosestActor(EndLocation, ClosestActor, HitActors)) return;

	HitActors.Add(ClosestActor);
	ImpactNormal = EndLocation - ClosestActor->GetActorLocation();
	ImpactNormal.Normalize();

	StartLocation = EndLocation;
	EndLocation = ClosestActor->GetActorLocation();

	if (ClosestActor->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
	{
		UAbilitySystemComponent* TargetACS = Cast<IAbilitySystemInterface>(ClosestActor)->GetAbilitySystemComponent();
		
		const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, 1);
		EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Calculation.Damage"), GetScaledDamage());
		ApplyAbilityTagsToGameplayEffectSpec(*EffectSpecHandle.Data.Get(), GetCurrentAbilitySpec());

		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data, TargetACS);
	}

	ChainLightning(StartLocation, EndLocation, ImpactNormal, Amount, HitActors);
}

FVector UChainLightningAbility::GetRandomUnitVectorInCone(const FVector& ConeDir, float MinAngle, float MaxAngle)
{
	// Convert the angles from degrees to radians
	const float MinAngleRadians = FMath::DegreesToRadians(MinAngle);
	const float MaxAngleRadians = FMath::DegreesToRadians(MaxAngle);

	// Generate a random direction vector
	const FVector RandomDirection = FMath::VRand();

	// Project the random direction onto the cone's plane
	FVector ProjectedDirection = RandomDirection - ConeDir * (RandomDirection | ConeDir);

	// Normalize the projected direction
	ProjectedDirection.Normalize();

	// Generate a random angle within the specified range
	const float RandomAngle = FMath::FRandRange(MinAngleRadians, MaxAngleRadians);

	// Create a quaternion representing the rotation around the cone's direction
	const FQuat RotationQuat = FQuat(ConeDir, RandomAngle);

	// Rotate the projected direction by the quaternion
	FVector RotatedDirection = RotationQuat.RotateVector(ProjectedDirection);

	// Normalize the final direction to ensure it's a unit vector
	RotatedDirection.Normalize();

	return RotatedDirection;
}
