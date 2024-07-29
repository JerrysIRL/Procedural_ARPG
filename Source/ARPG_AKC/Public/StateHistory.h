// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CellStateChange.h"
#include "StateHistory.generated.h"

class AWorldGenerationGameModeBase;
struct FTileVariant;
/**
 * 
 */
UCLASS()
class ARPG_AKC_API UStateHistory : public UObject
{
	GENERATED_BODY()
	
public:
	UStateHistory();
	
	UPROPERTY()
	TArray<UCellStateChange*> Changes;
	UPROPERTY()
	TSet<UCell*> CellsAdjacentToCollapsedCells;
	UPROPERTY()
	UCell* CollapsedCell;
	
	void SaveState(const TMap<UCell*, TArray<FTileVariant>>& CellStates, const TSet<UCell*>& InCellsAdjacentToCollapsedCells, UCell* InCollapsedCell);
	void RestoreState(AWorldGenerationGameModeBase* WorldGenerationGameModeBase);
};