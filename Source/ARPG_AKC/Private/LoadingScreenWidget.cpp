// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenWidget.h"


void ULoadingScreenWidget::Update(const int32 TotalTiles, const int32 CurrentTiles) const
{
	const float Percent = 1.0f - static_cast<float>(CurrentTiles) / TotalTiles;
	ProgressBar->SetPercent(Percent);

	const FText CurrentTilesText = FText::AsNumber(TotalTiles - CurrentTiles);
	const FText TotalTilesText = FText::AsNumber(TotalTiles);
	const FText TilesText = FText::Format(FText::FromString("{0} / {1}"), CurrentTilesText, TotalTilesText);
	TilesSpawned->SetText(TilesText);
}

void ULoadingScreenWidget::SetLevelName(const FText& Name) const
{
	LevelName->SetText(Name);
}

void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("Progress_Bar")));
	LevelName = Cast<UTextBlock>(GetWidgetFromName(TEXT("Level_Name")));
	TilesSpawned = Cast<UTextBlock>(GetWidgetFromName(TEXT("Tiles_Spawned")));
}
