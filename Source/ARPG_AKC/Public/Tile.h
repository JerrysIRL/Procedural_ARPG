// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileEnums.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UCell;
class ATile;

UENUM()
enum class ETileType : uint8
{
	TransitionTile,
	Beach,
	Forest,
	Desert
};

USTRUCT(BlueprintType)
struct FMeshAndWeight
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Weight = 1.f;
};

USTRUCT(BlueprintType)
struct FTileMultiplier
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
	TSubclassOf<ATile> TileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
	float Multiplier = 1.f;
};

USTRUCT(BlueprintType)
struct FSpawnArea
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector2D Origin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector2D Extent;
};

USTRUCT(BlueprintType)
struct FTileVariant
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
	TSubclassOf<ATile> TileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
	int32 Rotations = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
	float Weight = 1.f;

	bool operator==(const FTileVariant& Other) const
	{
		return TileClass.GetDefaultObject() == Other.TileClass.GetDefaultObject() && Rotations == Other.Rotations;
	}
	
	bool operator!=(const FTileVariant& Other) const
	{
		return !(*this == Other);
	}
};

UCLASS(Abstract, Blueprintable)
class ARPG_AKC_API ATile : public AActor
{
	GENERATED_BODY()

public:	
	ATile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
	ETileType TileType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
	float BaseWeight = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	/**
	 *	Holds the material types of a tiles edges.
	 *	The array starts at left of north side and goes clockwise around the tile.
	 *	
	 *			    N
	 * 		     1     2
	 * 		  #############
	 * 		8 ############# 3
	 * 	 W    #############    E
	 * 		7 ############# 4
	 * 		  #############
	 * 		     6     5
	 * 		        S
	 */	
	UPROPERTY(EditDefaultsOnly, Category = "Tile")
	TArray<EMaterialTypes> SideMaterials;
	
	ETileVariations GetTileVariations() const { return TileVariations; }
	float GetMultiplier(const TSubclassOf<ATile> Tile) const;

	UFUNCTION(BlueprintCallable)
	bool IsBlocked() const { return bIsBlocked;}

	UFUNCTION(BlueprintNativeEvent)
	float ApplyUniqueWeightChanges(UCell* OwnerCell, float Weight, int32 GridSize);

protected:
	virtual void BeginPlay() override;
	FTransform GenerateTransform(FVector2D BoxOrigin, FVector2D BoxExtent, float ScaleMin, float ScaleMax) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	bool bIsBlocked = false;
	
	virtual bool ShouldTickIfViewportsOnly() const override;
	void CalculateRotatedBox(FVector2D& BoxOrigin, FVector2D& BoxExtent) const;

	virtual void PostInitializeComponents() override;
	void Initialize();

	UPROPERTY(EditDefaultsOnly, Category = "Tile")
	TArray<FTileMultiplier> NeighborMultipliers;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile")
	ETileVariations TileVariations;
	
	UPROPERTY(EditDefaultsOnly, Category="Prop Spawning")
	TArray<FMeshAndWeight> Props;
	UPROPERTY(EditDefaultsOnly, Category="Prop Spawning")
	int32 PercentChanceToSpawnProp = 0;
	UPROPERTY(EditDefaultsOnly, Category="Prop Spawning")
	TArray<FSpawnArea> PropSpawnAreas;
	UPROPERTY(EditDefaultsOnly, Category="Prop Spawning")
	float PropScaleMin = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category="Prop Spawning")
	float PropScaleMax = 1.2f;
	
	UPROPERTY(EditDefaultsOnly, Category="Foliage Spawning")
	TArray<UStaticMesh*> Foliage;
	UPROPERTY(EditDefaultsOnly, Category="Foliage Spawning")
	int32 AmountOfFoliage = 0;
	UPROPERTY(EditDefaultsOnly, Category="Foliage Spawning")
	TArray<FSpawnArea> FoliageSpawnAreas;
	UPROPERTY(EditDefaultsOnly, Category="Foliage Spawning")
	float FoliageScaleMin = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category="Foliage Spawning")
	float FoliageScaleMax = 1.2f;
	
	void SpawnProp();
	void SpawnFoliage();
};