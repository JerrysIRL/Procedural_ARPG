#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "LootTable.generated.h"

USTRUCT(BlueprintType)
struct FLootTable : public FTableRowBase
{
	GENERATED_BODY()

	FLootTable(): ItemDataAsset(nullptr), Weight(0)
	{
	}

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	UItemDataAsset* ItemDataAsset;

	UPROPERTY(EditAnywhere)
	float Weight;
	
};
