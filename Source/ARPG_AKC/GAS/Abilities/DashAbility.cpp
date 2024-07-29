// Fill out your copyright notice in the Description page of Project Settings.


#include "DashAbility.h"

#include "MacroHelper.h"
#include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UDashAbility::UDashAbility()
{
	Name = "Dash";	
}

void UDashAbility::StartDash(FVector& EndLocation)
{
	const float CalculatedDuration = FVector::Distance(GetAvatarActorFromActorInfo()->GetActorLocation(), EndLocation) / Speed;
	
	FTimerHandle TimerHandle_Lerp;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(
		this,
		FName("Lerp"),
		GetAvatarActorFromActorInfo()->GetActorLocation(),
		EndLocation, 
		CalculatedDuration
		);

	StartTime = GetWorld()->GetTimeSeconds();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Lerp, TimerDelegate, 0.01f, true, 0.f);
	GetAvatarActorFromActorInfo()->SetActorEnableCollision(false);
}

void UDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndThisAbility();
	}

	AActor* Actor = GetAvatarActorFromActorInfo();
	
	FVector ActorLocation = Actor->GetActorLocation();

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	FVector WorldLocation, WorldDirection;
	
	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FHitResult HitResult;
	bool bGroundHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			WorldLocation,
			WorldLocation + WorldDirection * 10000.f,
			ECC_GameTraceChannel4
			);

	const FVector PlaneOrigin(0.0f, 0.0f, (bGroundHit ? HitResult.Location.Z : ActorLocation.Z) + 75.f);

	FVector TargetLocation = FMath::LinePlaneIntersection(
			WorldLocation,
			WorldLocation + WorldDirection,
			PlaneOrigin,
			FVector::UpVector);
	

	const FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
	
	FVector Direction = Rotator.Vector();
	
	Actor->SetActorRotation(Rotator);


	if (FVector::Distance(ActorLocation, TargetLocation) < Length)
	{
		StartDash(TargetLocation);
		return;
	}

	FVector EndLocation = ActorLocation + Direction * Length;
	EndLocation.Z = TargetLocation.Z;
	
	StartDash(EndLocation);
}

void UDashAbility::Lerp(const FVector& Start, const FVector& End, const float Duration)
{
	const float ElapsedSeconds = GetWorld()->GetTimeSeconds() - StartTime;

	if (ElapsedSeconds < Duration)
	{
		float Alpha = ElapsedSeconds / Duration;

		if (IsValid(CurveFloat))
		{
			Alpha = CurveFloat->GetFloatValue(Alpha);
		}

		GetAvatarActorFromActorInfo()->SetActorLocation(FMath::Lerp(Start, End, Alpha));
		return;
	}

	GetAvatarActorFromActorInfo()->SetActorLocation(End);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	GetAvatarActorFromActorInfo()->SetActorEnableCollision(true);
	EndThisAbility();
}
