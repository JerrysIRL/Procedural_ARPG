// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\StateHistory.h"
#include "Cell.h"
#include "WorldGenerationGameModeBase.h"

UStateHistory::UStateHistory()
{
}

void UStateHistory::SaveState(const TMap<UCell*, TArray<FTileVariant>>& CellStates, const TSet<UCell*>& InCellsAdjacentToCollapsedCells, UCell* InCollapsedCell)
{
	TArray<UCellStateChange*> CurrentStateChanges;
	for (const auto CellsAndPossibleTiles : CellStates)
	{
		UCellStateChange* Change = NewObject<UCellStateChange>();
		Change->Cell = CellsAndPossibleTiles.Key;
		Change->PossibleTileVariants = CellsAndPossibleTiles.Value;
		CurrentStateChanges.Add(Change);
	}
	Changes = CurrentStateChanges;

	CellsAdjacentToCollapsedCells = InCellsAdjacentToCollapsedCells;
	CollapsedCell = InCollapsedCell;
}

void UStateHistory::RestoreState(AWorldGenerationGameModeBase* WorldGenerationGameModeBase)
{
	for (UCellStateChange* Change : Changes)
	{
		Change->Cell->SetPossibleTilesAndWeights(Change->PossibleTileVariants);
		Change->ConditionalBeginDestroy();
	}
	Changes.Empty();
	const FTileVariant BannedTileVariant = CollapsedCell->GetCollapsedTile();
	CollapsedCell->Reset();
	CollapsedCell->RemovePossibleTile(BannedTileVariant);

	WorldGenerationGameModeBase->CellsAdjacentToCollapsedCells = CellsAdjacentToCollapsedCells;
}