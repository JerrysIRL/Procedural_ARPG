// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/LevelTileManager.h"

#include "BaseEnemy.h"
#include "MyGameInstance.h"
#include "ARPG_AKC/BaseItem.h"
#include "ARPG_AKC/LootTable.h"
#include "ARPG_AKC/Enemy/EnemyDeathManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


ALevelTileManager::ALevelTileManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelTileManager::BeginPlay()
{
	Super::BeginPlay();
	EnemyDeathManager = NewObject<UEnemyDeathManager>(this);
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		const FName CurrentLevelName = GetWorld()->GetFName();
		if (!GI->GetLevelData(CurrentLevelName))
		{
			GI->StoreLevelData(CurrentLevelName, GridData);
			const FString IsIslandString = IsIsland ? TEXT("True") : TEXT("False");
			FString StringOptions = "LevelToGenerate=" + CurrentLevelName.ToString() + "?IsIsland=" + IsIslandString;
			UGameplayStatics::OpenLevel(this, FName("WFCLoadingScreenLevel"), true, StringOptions);
			return;
		}

		GridData = *GI->GetLevelData(CurrentLevelName);
	}
	UpdateRenderedTiles();



	NotSpawnedCells = GridData.Grid;
	
	EnemyDeathManager->OnEnemyDied.AddDynamic(this, &ALevelTileManager::GenerateLoot);
	// // Music player
	// if(IsValid(MainMusicCue))w
	// {
	// 	UGameplayStatics::PlaySound2D(GetWorld(), MainMusicCue, VolumeMultiplier, 1.0f, 0.0f);
	// }
}

void ALevelTileManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateRenderedTiles();
	//const double TimeBudget = 0.011f; // 30fps
	//const double StartTime = FPlatformTime::Seconds();
	//
	//while (FPlatformTime::Seconds() - StartTime < TimeBudget)
	//{
	//	if (NotSpawnedCells.IsEmpty()) return;
	//	SpawnTileForCell(NotSpawnedCells[0]);
	//	NotSpawnedCells.RemoveAt(0);
	//}
}

void ALevelTileManager::UpdateRenderedTiles()
{
	const FVector2D NewLocation = WorldToGrid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());
	if (PlayerLocation == NewLocation)
		return;

	PlayerLocation = NewLocation;

	TArray<UCell*> CellsInView;

	for (int X = -3; X < 4; X++)
	{
		for (int Y = -3; Y < 4; Y++)
		{
			UCell* Cell = GetCell(PlayerLocation + FVector2D(X, Y));
			if (Cell)
				CellsInView.Add(Cell);
		}
	}

	for (UCell* Cell : CellsInView)
	{
		if (Cell->CollapsedTile) continue;
		SpawnTileForCell(Cell);
	}

	for (UCell* OldCell : VisibleCells)
	{
		if (CellsInView.Contains(OldCell)) continue;
		if (!OldCell->CollapsedTile) continue;
		OldCell->CollapsedTile->SetActorHiddenInGame(true);
	}

	for (UCell* NewCell : CellsInView)
	{
		if (VisibleCells.Contains(NewCell)) continue;
		if (!NewCell->CollapsedTile) continue;
		NewCell->CollapsedTile->SetActorHiddenInGame(false);
	}

	VisibleCells = CellsInView;
}

void ALevelTileManager::SpawnTileForCell(UCell* Cell) const
{
	if (!Cell || !Cell->bIsCollapsed) return;

	const FVector Location = GridToWorld(Cell->GridPosition);
	const FRotator Rotation = FRotator(0, -Cell->GetCollapsedTile().Rotations * 90.0f, 0);
	const FVector Scale = FVector(1, 1, 1);


	const FTransform Transform = FTransform(Rotation, Location, Scale);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ATile* Tile = GetWorld()->SpawnActor<ATile>(Cell->GetCollapsedTile().TileClass, Transform, SpawnParameters);
	Cell->CollapsedTile = Tile;
}

FVector ALevelTileManager::GridToWorld(FVector2D GridPosition)
{
	return FVector(GridPosition.X * 1000.0f, GridPosition.Y * 1000.0f, 0.0f);
}

FVector2D ALevelTileManager::WorldToGrid(const FVector& Location)
{
	const int32 X = (static_cast<int>(Location.X) + 500) / 1000 * 1000;
	const int32 Y = (static_cast<int>(Location.Y) + 500) / 1000 * 1000;
	return FVector2D(X / 1000, Y / 1000);
}

UCell* ALevelTileManager::GetCell(const FVector2D& GridPosition) const
{
	if (GridPosition.X < 0 || GridPosition.X >= GridData.GridSize || GridPosition.Y < 0 || GridPosition.Y >= GridData.GridSize)
		return nullptr;

	const int32 Index = GridPosition.X * GridData.GridSize + GridPosition.Y;
	return GridData.Grid[Index];
}

void ALevelTileManager::GenerateLoot(ABaseEnemy* Enemy)
{
	FVector EnemyLocation = Enemy->GetActorLocation();

	if (!LootTable) return;
	const FString ContextString(TEXT("Loot Table Context"));
	const TArray<FName> RowNames = LootTable->GetRowNames();
	if (RowNames.Num() == 0) return;

	float TotalWeight = 0;
	for (const FName RowName : RowNames)
	{
		if (const FLootTable* LootRow = LootTable->FindRow<FLootTable>(RowName, ContextString))
		{
			TotalWeight += LootRow->Weight;
		}
	}

	for (int i = 0; i < NumItemsToSpawn;)
	{
		const float RandomWeight = FMath::FRandRange(0, TotalWeight);

		UE_LOG(LogTemp, Warning, TEXT("RandomWeight in loot: %f"), RandomWeight);

		float CurrentWeight = 0;

		for (const FName RowName : RowNames)
		{
			if (const FLootTable* LootRow = LootTable->FindRow<FLootTable>(RowName, ContextString))
			{
				CurrentWeight += LootRow->Weight;

				if (CurrentWeight > RandomWeight)
				{
					if (!IsValid(LootRow->ItemDataAsset)) // No loot row taken into consideration
					{
						i++;
						break;
					}

					if (UItemDataAsset* SelectedItem = LootRow->ItemDataAsset)
					{
						SpawnLootAroundEnemy(SelectedItem, i, EnemyLocation);
						break;
					}
					break;
				}
			}
		}
	}
}

void ALevelTileManager::SpawnLootAroundEnemy(UItemDataAsset* ItemData, int32& NextItem, const FVector EnemyLocation)
{
	const FVector SpawnLocation = FVector(EnemyLocation.X, EnemyLocation.Y, GetActorLocation().Z);

	if (ItemData && BaseItemActor)
	{
		FItemGenericInfo ItemInfo = UDataLibrary::GenerateItem(ItemData->ItemGenericInfo, NextItem);

		if (ABaseItem* NewItem = GetWorld()->SpawnActor<ABaseItem>(BaseItemActor, SpawnLocation, FRotator::ZeroRotator))
		{
			NewItem->GenerateItem(ItemInfo);
		}
	}
}
