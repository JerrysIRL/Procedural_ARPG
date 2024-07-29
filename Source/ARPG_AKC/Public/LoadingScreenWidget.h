// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "LoadingScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Update(int32 TotalTiles, int32 CurrentTiles) const;
	void SetLevelName(const FText& LevelName) const;

protected:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LoadingScreenWidget")
	UProgressBar* ProgressBar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LoadingScreenWidget")
	UTextBlock* LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LoadingScreenWidget")
	UTextBlock* TilesSpawned;
};
