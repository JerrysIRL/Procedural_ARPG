// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\WorldGenerationGameModeBase.h"
#include "Cell.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

AWorldGenerationGameModeBase::AWorldGenerationGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorldGenerationGameModeBase::Initialize()
{
	CreateGrid();
	if (IsIsland)
	{
		TArray<FTileVariant> OceanTileVariants;
		CreateTileVariants(OceanTiles, OceanTileVariants);
		TArray<FTileVariant> Test;
		Test.Add(OceanTileVariants[0]);
		for (int32 i = 0; i < GridSize; ++i)
		{
			for (int32 j = 0; j < GridSize; ++j)
			{
				if (i < 2 || j < 2 || i > GridSize-3 || j > GridSize-3)
				{
					UCell* Cell = GetCell(FVector2D(i, j));
					Cell->SetCollapsedTile(Test[0]);
					Propagate(Cell);
					CellsAdjacentToCollapsedCells.Remove(Cell);
					IterationsLeft--;
				}
			}
		}
	}
	else
	{
		UCell* FirstCell = GetCell(FVector2D(GridSize / 2, GridSize / 2));
        FirstCell->SetCollapsedTile(AllowedTileVariants[1]);
        Propagate(FirstCell);
        IterationsLeft--;
	}
	if (LoadingScreenWidgetClass)
	{
		LoadingScreenWidget = CreateWidget<ULoadingScreenWidget>(GetWorld(), LoadingScreenWidgetClass);
		if (LoadingScreenWidget)
		{
			LoadingScreenWidget->AddToViewport();
			const FString GeneratedLevel = UGameplayStatics::ParseOption(OptionsString, "LevelToGenerate");
			const FString IsWater = UGameplayStatics::ParseOption(OptionsString, "IsIsland");
			const FText LevelName = FText::FromString(GeneratedLevel);
			LoadingScreenWidget->SetLevelName(LevelName);
		}
	}
}

void AWorldGenerationGameModeBase::Restart()
{
	UGameplayStatics::OpenLevel(this, FName("WFCLoadingScreenLevel"), true, OptionsString);
	
	//for (UCell* Cell : Grid)
	//{
	//	Cell->ConditionalBeginDestroy();
	//}
	//Grid.Empty();
	//LoadingScreenWidget->RemoveFromParent();
	//LoadingScreenWidget = nullptr;
	//Initialize();
}

void AWorldGenerationGameModeBase::CreateTileVariants(TArray<TSubclassOf<ATile>> TileTypes, TArray<FTileVariant>& OutTileVariants)
{
	for (TSubclassOf<ATile> TileType : TileTypes)
	{
		const ETileVariations TileVariations = TileType.GetDefaultObject()->GetTileVariations();
		const float Weight = TileType.GetDefaultObject()->BaseWeight;

		switch (TileVariations)
		{
		case ETileVariations::None:
			OutTileVariants.Add({TileType, 0, Weight});
			break;
	
		case ETileVariations::Rotate90:
			OutTileVariants.Add({TileType, 0, Weight});
			OutTileVariants.Add({TileType, 1, Weight});
			break;
	
		case ETileVariations::RotateAll:
			OutTileVariants.Add({TileType, 0, Weight});
			OutTileVariants.Add({TileType, 1, Weight});
			OutTileVariants.Add({TileType, 2, Weight});
			OutTileVariants.Add({TileType, 3, Weight});
			break;
	
		default:
			OutTileVariants.Add({TileType, 0, Weight});
			break;
		}
	}
}

void AWorldGenerationGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		const FName GeneratedLevel = *UGameplayStatics::ParseOption(OptionsString, "LevelToGenerate");
		const FLevelGridData* GridData = GI->GetLevelData(GeneratedLevel);
		AllowedTiles = GridData->AllowedTiles;
		GridSize = GridData->GridSize + 4;
		const FString IsIslandString = UGameplayStatics::ParseOption(OptionsString, "IsIsland");
		IsIsland = FCString::ToBool(*IsIslandString);
	}

	TotalIterations = GridSize * GridSize;
	IterationsLeft = TotalIterations;
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CreateTileVariants(AllowedTiles, AllowedTileVariants);
	Initialize();
}

void AWorldGenerationGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const double StartTime = FPlatformTime::Seconds();
	const double TimeBudget = 0.033f; // 30fps
	
	while (FPlatformTime::Seconds() - StartTime < TimeBudget)
	{
		ProcessGenerationStep();
		if (IterationsLeft <= 0)
		{
			FinishGeneration();
			return;
		}
	}

	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->Update(TotalIterations, IterationsLeft);
	}
}

void AWorldGenerationGameModeBase::CreateGrid()
{
	Grid.SetNum(GridSize * GridSize);
	
	for (int32 i = 0; i < GridSize; ++i)
	{
		for (int32 j = 0; j < GridSize; ++j)
		{
			UCell* NewCell = NewObject<UCell>();
			NewCell->Initialize(FVector2D(i, j), AllowedTileVariants);
			const int32 Index = i * GridSize + j;
			Grid[Index] = NewCell;
		}
	}
	for (UCell* Cell : Grid)
	{
		Cell->Neighbors = GetNeighbors(Cell->GridPosition);
	}
}

void AWorldGenerationGameModeBase::FinishGeneration()
{
	
	const FString GeneratedLevel = UGameplayStatics::ParseOption(OptionsString, "LevelToGenerate");
	const FName LevelName(*GeneratedLevel);
	const FLevelGridData GridData = FLevelGridData(Grid, GridSize);
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		GI->StoreLevelData(LevelName, GridData);
	}
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Failed collapsed cells: %d"), FailedCollapses));
	
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void AWorldGenerationGameModeBase::ProcessGenerationStep()
{
	TArray<UCell*> MinimumEntropyCells = GetMinimumEntropyCells();
	const int32 RandNum = FMath::RandRange(0, MinimumEntropyCells.Num() - 1);
	UCell* LowestEntropyCell = MinimumEntropyCells[RandNum];
	TArray<FTileVariant> PossibleTileVariants = LowestEntropyCell->GetPossibleTileVariants();
	
	for (const UCell* Neighbor : LowestEntropyCell->Neighbors)
	{
		if (!Neighbor->bIsCollapsed) continue;
		for (auto& PossibleTileVariant : PossibleTileVariants)
		{
			const float Multiplier = Neighbor->GetCollapsedTile().TileClass.GetDefaultObject()->GetMultiplier(PossibleTileVariant.TileClass);
			PossibleTileVariant.Weight *= Multiplier;
		}
	}

	for (int32 i = PossibleTileVariants.Num() - 1; i >= 0; --i)
	{
		auto& PossibleTileVariant = PossibleTileVariants[i];
		const float ModifiedWeight = PossibleTileVariant.TileClass.GetDefaultObject()->ApplyUniqueWeightChanges(LowestEntropyCell, PossibleTileVariant.Weight, GridSize);
		PossibleTileVariant.Weight = ModifiedWeight;
		if (ModifiedWeight < 0)
		{
			PossibleTileVariants.RemoveAt(i);
		}
	}

	if (PossibleTileVariants.Num() == 0)
	{
		FailedCollapses++;
		RestorePreviousIteration();
		return;
	}
	
	LowestEntropyCell->SetPossibleTilesAndWeights(PossibleTileVariants);
	LowestEntropyCell->Collapse();
	CellsAdjacentToCollapsedCells.Remove(LowestEntropyCell);
	Propagate(LowestEntropyCell);
	IterationsLeft--;
}

void AWorldGenerationGameModeBase::RestorePreviousIteration()
{
	if (CellsStateHistories.Num() == 0)
	{
		Restart();
		return;
	}
	IterationsLeft++;
	UStateHistory* StateHistory = CellsStateHistories.Pop();
	StateHistory->RestoreState(this);
}

TArray<UCell*> AWorldGenerationGameModeBase::GetMinimumEntropyCells() const
{
	TArray<UCell*> MinimumEntropyCells;
	int32 LowestEntropyValue = AllowedTileVariants.Num();
	
	for (const UCell* Cell : CellsAdjacentToCollapsedCells)
	{
		if (LowestEntropyValue > Cell->GetEntropy())
		{
			LowestEntropyValue = Cell->GetEntropy();
		}
	}

	for (UCell* Cell : CellsAdjacentToCollapsedCells)
	{
		if (LowestEntropyValue == Cell->GetEntropy())
		{
			MinimumEntropyCells.Add(Cell);
		}
	}

	return MinimumEntropyCells;
}

void AWorldGenerationGameModeBase::Propagate(UCell* CollapsedCell)
{
	UStateHistory* CurrentStateHistory = NewObject<UStateHistory>();
	TMap<UCell*, TArray<FTileVariant>> CellStates;
	TQueue<UCell*> CellsToUpdate;
	TSet<UCell*> EnqueuedCells;

	CellsToUpdate.Enqueue(CollapsedCell);
	EnqueuedCells.Add(CollapsedCell);

	while (!CellsToUpdate.IsEmpty())
	{
		UCell* CurrentCell;
		CellsToUpdate.Dequeue(CurrentCell);
		
		for (UCell* Neighbor : CurrentCell->Neighbors)
		{
			if (Neighbor->bIsCollapsed || EnqueuedCells.Contains(Neighbor)) continue;
			CellsAdjacentToCollapsedCells.Add(Neighbor);

			TArray<FTileVariant> PossibleTilesAndWeightsCopy;
			
			for (const FTileVariant& TileVariant : Neighbor->GetPossibleTileVariants())
			{
				FTileVariant NewTileVariant;
				NewTileVariant.TileClass = TileVariant.TileClass;
				NewTileVariant.Rotations = TileVariant.Rotations;
				NewTileVariant.Weight = TileVariant.Weight;
				PossibleTilesAndWeightsCopy.Add(NewTileVariant);
			}
			
			if (ApplyDirectionalConstraints(Neighbor, CurrentCell))
			{
				CellsToUpdate.Enqueue(Neighbor);
				EnqueuedCells.Add(Neighbor);
				CellStates.Add(Neighbor, PossibleTilesAndWeightsCopy);
			}
		}
	}

	CurrentStateHistory->SaveState(CellStates, CellsAdjacentToCollapsedCells, CollapsedCell);
	CellsStateHistories.Push(CurrentStateHistory);
	if (CellsStateHistories.Num() > 50)
	{
		CellsStateHistories.RemoveAt(0);
	}
}

bool AWorldGenerationGameModeBase::ApplyDirectionalConstraints(UCell* CellToUpdate, const UCell* CollapsedNeighborCell)
{
	if (!CellToUpdate || CellToUpdate->bIsCollapsed) return false;
	if (!CollapsedNeighborCell || !CollapsedNeighborCell->bIsCollapsed) return false;
	
	TArray<FTileVariant> NewPossibleTilesVariants;
	const FVector2D Direction = GetDirection(CellToUpdate->GridPosition, CollapsedNeighborCell->GridPosition);
	
	if (Direction == FVector2D(0, 0)) return false;
	
	for (auto PossibleTileVariant : CellToUpdate->GetPossibleTileVariants())
	{
		if (!PossibleTileVariant.TileClass) continue;
		if (CanBeNeighbor(PossibleTileVariant, CollapsedNeighborCell->GetCollapsedTile(), Direction))
		{
			NewPossibleTilesVariants.Add(PossibleTileVariant);
		}
	}


	const bool bFlag = NewPossibleTilesVariants.Num() != CellToUpdate->GetEntropy();
	
	CellToUpdate->SetPossibleTilesAndWeights(NewPossibleTilesVariants);
	
	return bFlag;
}

bool AWorldGenerationGameModeBase::CanBeNeighbor(const FTileVariant& TileVariant, const FTileVariant& NeighborTileVariant, const FVector2D& DirectionToNeighbor)
{
	ATile* Tile = TileVariant.TileClass.GetDefaultObject();
	ATile* NeighborTile = NeighborTileVariant.TileClass.GetDefaultObject();
	const int32 TileRotationsOffset = TileVariant.Rotations * 2;
	const int32 NeighborTileRotationsOffset = NeighborTileVariant.Rotations * 2;
	
	if (DirectionToNeighbor.Equals(FVector2D(1, 0))
	&& Tile->SideMaterials[(0 + TileRotationsOffset) % 8] == NeighborTile->SideMaterials[(5 + NeighborTileRotationsOffset) % 8]
	&& Tile->SideMaterials[(1 + TileRotationsOffset) % 8] == NeighborTile->SideMaterials[(4 + NeighborTileRotationsOffset) % 8])
	{
		return true;
	}
	
	if (DirectionToNeighbor.Equals(FVector2D(0, 1))
	&& Tile->SideMaterials[(2 + TileRotationsOffset) % 8] == NeighborTile->SideMaterials[(7 + NeighborTileRotationsOffset) % 8]
	&& Tile->SideMaterials[(3 + TileRotationsOffset) % 8] == NeighborTile->SideMaterials[(6 + NeighborTileRotationsOffset) % 8])
	{
		return true;
	}

	if (DirectionToNeighbor.Equals(FVector2D(-1, 0))
	&& Tile->SideMaterials[(4 + TileRotationsOffset) % 8] == NeighborTile->SideMaterials[(1 + NeighborTileRotationsOffset) % 8]
	&& Tile->SideMaterials[(5 + TileRotationsOffset) % 8] == NeighborTile->SideMaterials[(0 + NeighborTileRotationsOffset) % 8])
	{
		return true;
	}

	if (DirectionToNeighbor.Equals(FVector2D(0, -1))
	&& Tile->SideMaterials[(6 + TileRotationsOffset) % 8] == NeighborTile->SideMaterials[(3 + NeighborTileRotationsOffset) % 8]
	&& Tile->SideMaterials[(7 + TileRotationsOffset) % 8] == NeighborTile->SideMaterials[(2 + NeighborTileRotationsOffset) % 8])
	{
		return true;
	}
	return false;
}

TArray<UCell*> AWorldGenerationGameModeBase::GetNeighbors(FVector2D GridPosition) const
{
	
	TArray<UCell*> Neighbors;
	TArray Directions =
	{
		FVector2D(1, 0),  // North
		FVector2D(0, 1),  // East
		FVector2D(-1, 0), // South
		FVector2D(0, -1), // West
	};

	for (const FVector2D& Direction : Directions)
	{
		FVector2D NeighborPosition = GridPosition + Direction;
		UCell* NeighborCell = GetCell(NeighborPosition);
        
		if (!NeighborCell) continue;

		Neighbors.Add(NeighborCell);
	}
	return Neighbors;
}

UCell* AWorldGenerationGameModeBase::GetCell(const FVector2D& GridPosition) const
{
	if (GridPosition.X < 0 || GridPosition.X >= GridSize || GridPosition.Y < 0 || GridPosition.Y >= GridSize)
		return nullptr;

	const int32 Index = GridPosition.X * GridSize + GridPosition.Y;
	return Grid[Index];
}

FVector2D AWorldGenerationGameModeBase::GetDirection(const FVector2D From, const FVector2D To)
{
	const FVector2D Direction = To - From;
	
	if (FMath::Abs(Direction.X) + FMath::Abs(Direction.Y) != 1) return FVector2D(0, 0);

	return Direction;
}