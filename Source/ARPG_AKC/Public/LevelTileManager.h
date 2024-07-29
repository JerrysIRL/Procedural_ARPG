// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cell.h"
#include "Engine/LevelScriptActor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelTileManager.generated.h"

class UEnemyDeathManager;
class ABaseEnemy;
class USoundCue;

USTRUCT(BlueprintType)
struct FLevelGridData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<UCell*> Grid;

	UPROPERTY(EditDefaultsOnly)
	int32 GridSize = 100;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATile>> AllowedTiles;
};

class ABaseItem;
UCLASS()
class ARPG_AKC_API ALevelTileManager : public ALevelScriptActor
{
	GENERATED_BODY()

	ALevelTileManager();

	
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	TArray<UCell*> NotSpawnedCells;

	FVector2D PlayerLocation;
	void UpdateRenderedTiles();

	FTimerHandle TimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "GridData")
	FLevelGridData GridData;

	UPROPERTY(EditDefaultsOnly, Category = "GridData")
	bool IsIsland;

	UPROPERTY()
	TArray<UCell*> VisibleCells;
	
	void SpawnTileForCell(UCell* Cell) const;
	static FVector GridToWorld(FVector2D GridPosition);
	static FVector2D WorldToGrid(const FVector& Location);
	UCell* GetCell(const FVector2D& GridPosition) const;

public:
	virtual void BeginPlay() override;
	UPROPERTY()
	UEnemyDeathManager* EnemyDeathManager;

	UFUNCTION(BlueprintCallable)
	void GenerateLoot(ABaseEnemy* Enemy);
	
	UFUNCTION(BlueprintCallable)
	void SpawnLootAroundEnemy(UItemDataAsset* ItemData, int32& NextItem, const FVector EnemyLocation);

	UPROPERTY(EditAnywhere)
	UDataTable* LootTable;

	UPROPERTY(EditAnywhere)
	int NumItemsToSpawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseItem> BaseItemActor;

private:
	UPROPERTY(EditAnywhere, Category = "Music")
	USoundCue* MainMusicCue;

	UPROPERTY(EditAnywhere, Category = "Music")
	float VolumeMultiplier = 0.5f;
};
