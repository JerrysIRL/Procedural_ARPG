
// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUDWidget.h"

#include "InventoryWidget.h"
#include "ItemInfoWidget.h"
#include "ARPG_AKC/Widgets/ItemNameWidget.h"
#include "Kismet/GameplayStatics.h"

void UMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	
}

void UMainHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(ItemNameWidgetMap.Num() <= 0) return;

	for (auto ItemWidgetPair : ItemNameWidgetMap)
	{
		FVector2D ScreenPos;
		UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), ItemWidgetPair.Key->GetActorLocation(), ScreenPos);
		ItemWidgetPair.Value->SetPositionInViewport(ScreenPos - FVector2D(25, 0));
	}
}

void UMainHUDWidget::InitializeWidget()
{
	ItemInfoWidget->InitializeWidget();
	
	InventoryTooltip = CreateWidget<UItemInfoWidget>(this, ItemInfoWidget->GetClass());
	
	if (!IsValid(InventoryTooltip))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to duplicate ItemInfoWidget"));
	}
	InventoryTooltip->InitializeWidget();
	
}

void UMainHUDWidget::AddItemNameWidget(ABaseItem* Item)
{
	UItemNameWidget* ItemNameWidget = CreateWidget<UItemNameWidget>(this, ItemNameWidgetClass);
	
	if(ItemNameWidget == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create ItemNameWidget"));
		return;
	}
	
	ItemNameWidget->ItemName->SetText(Item->GenerationInfo.ItemTextData.ItemName);
	ItemNameWidget->BackgroundColor->SetColorAndOpacity(Item->GenerationInfo.SetColor());
	ItemNameWidgetMap.Add(Item, ItemNameWidget);
	ItemNameWidget->Item = Item;

	// Adding to viewport position so that it doesn't flash bomb the screen
	FVector2D ScreenPos;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), Item->GetActorLocation(), ScreenPos);
	ItemNameWidget->SetPositionInViewport(ScreenPos - FVector2D(25, 0));
	ItemNameWidget->AddToViewport(-1);
}

void UMainHUDWidget::RemoveItemNameWidget(ABaseItem* Item)
{
	if(ItemNameWidgetMap.Contains(Item))
		ItemNameWidgetMap.FindAndRemoveChecked(Item)->RemoveFromParent();
}


void UMainHUDWidget::ShowItemInfoWidget(ABaseItem* Item)
{
	ItemInfoWidget->OnItemHovered.Broadcast(Item->GenerationInfo);
	ItemInfoWidget->ItemAffixesDelegate.Broadcast(Item->AffixDataAssets);
	ItemInfoWidget->ItemBackgroundColorDelegate.Broadcast(Item->GenerationInfo.SetColor());
}

void UMainHUDWidget::HideItemInfoWidget()
{
	ItemInfoWidget->OnItemUnhovered.Broadcast();
}
