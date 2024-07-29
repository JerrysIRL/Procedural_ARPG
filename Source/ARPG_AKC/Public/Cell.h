// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "Cell.generated.h"


/**
 * 
 */
UCLASS()
class ARPG_AKC_API UCell : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsCollapsed = false;
	
	UPROPERTY(BlueprintReadOnly)
	FVector2D GridPosition;

	UPROPERTY(BlueprintReadOnly)
	ATile* CollapsedTile;

	void Initialize(FVector2D InGridPosition, const TArray<FTileVariant>& AllTileVariants);
	void Collapse();

	UPROPERTY(BlueprintReadOnly)
	TArray<UCell*> Neighbors;
	
	TArray<FTileVariant> GetPossibleTileVariants() const { return PossibleTileVariants; }
	int32 GetEntropy() const { return Entropy; }

	UFUNCTION(BlueprintCallable)
	FTileVariant GetCollapsedTile() const { return CollapsedTileVariant; }
	
	void SetPossibleTilesAndWeights(const TArray<FTileVariant>& InPossibleTileVariants);
	void SetCollapsedTile(const FTileVariant& TileVariant);
	void ResetPossibleTilesAndWeights(TArray<FTileVariant> AllTileVariants);
	void Reset();
	void RemovePossibleTile(const FTileVariant& TileVariant);

private:
	UPROPERTY()
	TArray<FTileVariant> PossibleTileVariants;
	UPROPERTY()
	FTileVariant CollapsedTileVariant;
	int32 Entropy;
};
