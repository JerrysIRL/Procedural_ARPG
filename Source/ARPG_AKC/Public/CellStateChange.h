// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CellStateChange.generated.h"

class UCell;
struct FTileVariant;

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UCellStateChange : public UObject
{
	GENERATED_BODY()
	UCellStateChange();
	
public:
	UPROPERTY()
	UCell* Cell;

	UPROPERTY()
	TArray<FTileVariant> PossibleTileVariants;
};

inline UCellStateChange::UCellStateChange()
{
}
