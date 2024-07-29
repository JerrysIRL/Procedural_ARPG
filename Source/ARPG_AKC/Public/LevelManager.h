// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPG_AKC/ItemDataAsset.h"
#include "Engine/LevelScriptActor.h"
#include "LevelManager.generated.h"

/**
 * 
 */

class ABaseItem;


UCLASS()
class ARPG_AKC_API ALevelManager : public ALevelScriptActor
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void GenerateLoot(const FVector2D& EnemyLocation);
	
	UFUNCTION(BlueprintCallable)
	void SpawnLootAroundEnemy(UItemDataAsset* ItemData, int32& NextItem, const FVector2D& EnemyLocation);

	UPROPERTY(EditAnywhere)
	UDataTable* LootTable;

	UPROPERTY(EditAnywhere)
	int NumItemsToSpawn = 10;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseItem> BaseItemActor;
protected:
	
	virtual void BeginPlay() override;
};
