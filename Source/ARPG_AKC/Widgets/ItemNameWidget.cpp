// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemNameWidget.h"

#include "ItemInfoWidget.h"
#include "MainGameHUD.h"
#include "MainHUDWidget.h"
#include "ARPG_AKC/BaseItem.h"
#include "Kismet/GameplayStatics.h"

void UItemNameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainGameHUD = Cast<AMainGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
}


void UItemNameWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if(IsValid(Item) && IsVisible())
		MainGameHUD->MainHUDWidget->ShowItemInfoWidget(Item);
}

void UItemNameWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	MainGameHUD->MainHUDWidget->HideItemInfoWidget();
}

FReply UItemNameWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		MainGameHUD->MainHUDWidget->HideItemInfoWidget();
		SetVisibility(ESlateVisibility::HitTestInvisible);
		Item->AddToInventory();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
