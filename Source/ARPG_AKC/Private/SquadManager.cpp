// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadManager.h"
#include "NavigationSystem.h"
#include "Tile.h"
#include "AI/NavigationSystemBase.h"
#include "AI/Navigation/NavigationDataInterface.h"
#include "ARPG_AKC/ARPG_AKCGameMode.h"
#include "Kismet/GameplayStatics.h"


void USquadManager::OnWorldBeginPlay(UWorld& InWorld)
{
	GameMode = Cast<AARPG_AKCGameMode>(GetWorld()->GetAuthGameMode());

	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	UE_LOG(LogTemp, Warning, TEXT("World Begin Play"));
	Super::OnWorldBeginPlay(InWorld);

	TArray<AActor*> tempArr;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASquad::StaticClass(), tempArr);
	for (auto squadActor : tempArr)
	{
		auto squad = Cast<ASquad>(squadActor);
		if (squad)
			Squads.Add(squad);
	}
	InWorld.GetTimerManager().SetTimer(DespawnerHandle, this, &USquadManager::HandleSquadDespawning, 1.0f, true, 1.0f);
	InWorld.GetTimerManager().SetTimer(SpawnerHandle, this, &USquadManager::HandleSquadSpawning, 1.0f, true, 1.0f);
}

bool USquadManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	auto World = Outer->GetWorld();
	if (!World)
		return false;

	if (Levels.Contains(World->GetName()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Creating system!"));
		return true;
	}

	return false;
}


void USquadManager::HandleSquadDespawning()
{
	float RadiusSqd = FMath::Square(DespawningRadius);
	for (auto Squad : Squads)
	{
		if (Squad->SquadMembers.Num() != 0)
		{
			if (FVector::DistSquaredXY(Player->GetActorLocation(), Squad->Center) < RadiusSqd)
				continue;

			FVector PlayerVelocity = Player->GetVelocity();
			if (PlayerVelocity.Length() < 1.0f)
				continue;

			FVector SquadToPlayer = Player->GetActorLocation() - Squad->Center;
			float DotProduct = FVector::DotProduct(PlayerVelocity, SquadToPlayer);
			if (DotProduct < 0)
				continue;
		}

		Squads.Remove(Squad);
		Squad->Destroy();
		break;
	}
}

FVector USquadManager::GetValidSquadPosition()
{
	FVector SpawnLocation = Player->GetActorLocation() + Player->GetActorForwardVector() * MinSpawnRadius;

	if (IsValidLocation(SpawnLocation))
		return SpawnLocation;

	FVector SpawnForward = Player->GetActorForwardVector();

	for (int i = 0; i <= 180; i += 20)
	{
		for (int j = 0; j < 2; j++)
		{
			FVector TestLocation = Player->GetActorLocation() + SpawnForward.RotateAngleAxis(i * (j == 0 ? 1 : -1), FVector::UpVector) * MinSpawnRadius;
			if (IsValidLocation(TestLocation))
				return TestLocation;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Did not find valid point"));
	return Player->GetActorLocation() + -Player->GetActorForwardVector() * MinSpawnRadius;;
}


void USquadManager::HandleSquadSpawning()
{
	if (Squads.Num() >= MaxActiveSquads)
		return;

	if (!GameMode)
		return;

	FVector SpawnLocation = GetValidSquadPosition();
	FNavLocation NavMeshLocation;
	if (auto NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
		if (NavSystem->GetMainNavData()->ProjectPoint(SpawnLocation, NavMeshLocation, FVector(100, 100, 1000), nullptr, nullptr))
			SpawnLocation = NavMeshLocation.Location;

	auto Squad = GetWorld()->SpawnActor<ASquad>(GameMode->SquadBlueprint, SpawnLocation, FRotator::ZeroRotator);
	Squad->SpawnSquad();
	Squads.Add(Squad);
}

bool USquadManager::IsValidLocation(FVector Location)
{
	if (FVector::Dist(Location, Player->GetActorLocation()) < MinSpawnRadius)
		return false;

	for (auto Squad : Squads)
	{
		if (FVector::Dist(Location, Squad->Center) < DistanceBetweenSquads)
			return false;
	}

	FHitResult HitResult;
	for (int i = 1; i <= 271; i += 90)
	{
		auto Degrees = FMath::DegreesToRadians(i);
		FVector AngleOffset = FVector(FMath::Cos(Degrees), FMath::Sin(Degrees), 0) * 350;
		FVector Position = Location + AngleOffset + (FVector::UpVector * 200);

		if (!GetWorld()->LineTraceSingleByChannel(HitResult, Position, -FVector::UpVector * TNumericLimits<float>::Max(), ECC_Visibility))
		{
			return false;
		}
		if (auto Tile = Cast<ATile>(HitResult.GetActor()))
		{
			if (Tile->IsBlocked())
				return false;
		}
	}

	return true;
}

float USquadManager::GetSpawnRadius() const
{
	return FMath::RandRange(MinSpawnRadius, MaxSpawnRadius);
}
