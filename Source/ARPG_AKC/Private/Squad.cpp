// Fill out your copyright notice in the Description page of Project Settings.


#include "Squad.h"
#include "Kismet/KismetMathLibrary.h"


ASquad::ASquad()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASquad::Destroyed()
{
	Super::Destroyed();
	for (const auto Member : SquadMembers)
	{
		Member->Destroy();
	}
	SquadMembers.Empty();
}

void ASquad::BeginPlay()
{
	Super::BeginPlay();
}

void ASquad::RemoveSquadMember(ABaseEnemy* EnemyToRemove)
{
	SquadMembers.Remove(EnemyToRemove);
	EnemyToRemove->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ASquad::SpawnSquad()
{
	int MediumEnemyCount = FMath::RandRange(1, 2);
	FActorSpawnParameters SpawnParams;
	FVector YOffset = FVector::UpVector * 150;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (int i = 0; i < MediumEnemyCount; i++)
	{
		auto MedEnemy = GetWorld()->SpawnActor<ABaseEnemy>(MediumEnemies[FMath::RandRange(0, MediumEnemies.Num() - 1)], GetActorLocation() + YOffset, FRotator::ZeroRotator, SpawnParams);

		SquadMembers.Add(MedEnemy);
		MedEnemy->OnPlayerDead.AddUniqueDynamic(this, &ASquad::RemoveSquadMember);
		MedEnemy->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}

	int SquadCount = FMath::RandRange(5, 6) - MediumEnemyCount;
	int StartDegrees = FMath::RandRange(0, 180);

	for (int i = 0; i < SquadCount; i++)
	{
		float Angle = FMath::DegreesToRadians(((float)i / SquadCount) * 360 + StartDegrees + FMath::RandRange(-20, 20));
		FVector SpawnOffset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0) * DistanceFromCenter;
		auto SmallEnemy = GetWorld()->SpawnActor<ABaseEnemy>(SmallEnemies[FMath::RandRange(0, SmallEnemies.Num() - 1)], GetActorLocation() + SpawnOffset + YOffset, UKismetMathLibrary::FindLookAtRotation(FVector::Zero(), SpawnOffset), SpawnParams);

		SquadMembers.Add(SmallEnemy);
		SmallEnemy->OnPlayerDead.AddUniqueDynamic(this, &ASquad::RemoveSquadMember);
		SmallEnemy->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
}


void ASquad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (SquadMembers.Num() < 0)
		return;

	Center = FVector::ZeroVector;
	for (auto Enemy : SquadMembers)
	{
		Center += Enemy->GetActorLocation();
	}

	Center /= SquadMembers.Num();
}
