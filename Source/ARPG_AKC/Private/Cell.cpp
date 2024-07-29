// Fill out your copyright notice in the Description page of Project Settings.


#include "Cell.h"
#include "Tile.h"

void UCell::Initialize(const FVector2D InGridPosition, const TArray<FTileVariant>& AllTileVariants)
{
	GridPosition = InGridPosition;
	ResetPossibleTilesAndWeights(AllTileVariants);
}

void UCell::Collapse()
{
	if (bIsCollapsed) return;
	if (PossibleTileVariants.Num() == 0)
		return;

	float TotalWeight = 0.0f;
	for (const FTileVariant TileVariant : PossibleTileVariants)
	{
		TotalWeight += TileVariant.Weight;
	}

	const float RandomWeight = FMath::FRandRange(0, TotalWeight);
	float CurrentWeight = 0;

	float SelectedIndex = 0;
	for (; SelectedIndex < PossibleTileVariants.Num(); ++SelectedIndex)
	{
		CurrentWeight += PossibleTileVariants[SelectedIndex].Weight;
		if (CurrentWeight >= RandomWeight) break;
	}
	
	if (SelectedIndex >= 0 && SelectedIndex < PossibleTileVariants.Num())
	{
		SetCollapsedTile(PossibleTileVariants[SelectedIndex]);
	}
}

void UCell::SetCollapsedTile(const FTileVariant& TileVariant)
{
	CollapsedTileVariant = TileVariant;
	bIsCollapsed = true;
}

void UCell::ResetPossibleTilesAndWeights(TArray<FTileVariant> AllTileVariants)
{
	TArray<FTileVariant> TileVariants;
	for (const FTileVariant TileVariant : AllTileVariants)
	{
		TileVariants.Add(TileVariant);
	}
	SetPossibleTilesAndWeights(TileVariants);
}

void UCell::Reset()
{
	CollapsedTileVariant = FTileVariant();
	bIsCollapsed = false;
}

void UCell::RemovePossibleTile(const FTileVariant& TileVariant)
{
	for (int i = 0; i < PossibleTileVariants.Num(); i++)
	{
		if (PossibleTileVariants[i] != TileVariant) continue;
		PossibleTileVariants.RemoveAt(i);
		return;
 	}
}


void UCell::SetPossibleTilesAndWeights(const TArray<FTileVariant>& InPossibleTileVariants)
{
	PossibleTileVariants = InPossibleTileVariants;
	Entropy = PossibleTileVariants.Num();
}
