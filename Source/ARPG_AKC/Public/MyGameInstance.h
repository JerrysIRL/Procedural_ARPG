// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelTileManager.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Data")
	TMap<FName, FLevelGridData> LevelGrids;

	void StoreLevelData(const FName LevelName, const FLevelGridData& GridData);
	
	FLevelGridData* GetLevelData(const FName LevelName);
};
