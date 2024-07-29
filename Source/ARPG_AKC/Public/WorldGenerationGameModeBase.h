// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Cell.h"
#include "StateHistory.h"
#include "LoadingScreenWidget.h"
#include "WorldGenerationGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class ARPG_AKC_API AWorldGenerationGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AWorldGenerationGameModeBase();
	void Initialize();
	void Restart();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	TArray<UStateHistory*> CellsStateHistories;

	UPROPERTY()
	TSet<UCell*> CellsAdjacentToCollapsedCells;

	bool bIsGenerating;
	int32 IterationsLeft;

	int32 FailedCollapses;

	int32 TotalIterations;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULoadingScreenWidget> LoadingScreenWidgetClass;

	UPROPERTY()
	ULoadingScreenWidget* LoadingScreenWidget;

	UPROPERTY()
	TArray<TSubclassOf<ATile>> AllowedTiles;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATile>> OceanTiles;
	
	UPROPERTY()
	TArray<FTileVariant> AllowedTileVariants;

	UPROPERTY()
	bool IsIsland;
	
	UPROPERTY()
	int32 GridSize = 100;

	void CreateGrid();
	void ProcessGenerationStep();
	void RestorePreviousIteration();
	
	void FinishGeneration();

	void Propagate(UCell* CollapsedCell);

	// Returns True if changes has been made.
	static bool ApplyDirectionalConstraints(UCell* CellToUpdate, const UCell* CollapsedNeighborCell);
	static bool CanBeNeighbor(const FTileVariant& TargetTile, const FTileVariant& NeighborTileVariant, const FVector2D& DirectionToNeighbor);
	TArray<UCell*> GetNeighbors(FVector2D GridPosition) const;
	UCell* GetCell(const FVector2D& GridPosition) const;
	// Returns (0, 0) if direction is invalid.
	static FVector2D GetDirection(const FVector2D From, const FVector2D To);
	TArray<UCell*> GetMinimumEntropyCells() const;

private:
	UPROPERTY()
	TArray<UCell*> Grid;

	static void CreateTileVariants(TArray<TSubclassOf<ATile>> TileTypes, TArray<FTileVariant>& OutTileVariants);

};
