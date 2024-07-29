#pragma once

#include "CoreMinimal.h"
#include "AffixDataAsset.h"
#include "Affixes.generated.h"

USTRUCT(Blueprintable)
struct FAffixes : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere)
	UAffixDataAsset* AffixDataAsset;
};
