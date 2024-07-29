// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

void UMyGameInstance::StoreLevelData(const FName LevelName, const FLevelGridData& GridData)
{
	LevelGrids.Add(LevelName, GridData);
}

FLevelGridData* UMyGameInstance::GetLevelData(const FName LevelName)
{
	return LevelGrids.Find(LevelName);
}