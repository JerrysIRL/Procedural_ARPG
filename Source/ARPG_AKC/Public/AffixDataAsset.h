// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AffixLibrary.h"
#include "Engine/DataAsset.h"
#include "AffixDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UAffixDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FAffixesInfo AffixesInfo;
};
