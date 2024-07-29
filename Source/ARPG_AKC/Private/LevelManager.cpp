// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManager.h"

#include "ARPG_AKC/BaseItem.h"
#include "ARPG_AKC/LootTable.h"

void ALevelManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelManager::GenerateLoot(const FVector2D& EnemyLocation)
{
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
	
	for (int i = 0; i < RowNames.Num();)
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
					if(!IsValid(LootRow->ItemDataAsset)) // No loot row taken into consideration
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

void ALevelManager::SpawnLootAroundEnemy(UItemDataAsset* ItemData, int32& NextItem, const FVector2D& EnemyLocation)
{
	const FVector SpawnLocation = FVector(EnemyLocation.X, EnemyLocation.Y, GetActorLocation().Z);
	
	if (ItemData && BaseItemActor)
	{
		if (ABaseItem* NewItem = GetWorld()->SpawnActor<ABaseItem>(BaseItemActor, SpawnLocation, FRotator::ZeroRotator))
		{
			//NewItem->GenerateItem(ItemData->ItemGenericInfo, NextItem);
		}
	}
}