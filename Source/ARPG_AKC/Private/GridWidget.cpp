#include "GridWidget.h"

#include "InventoryComponent.h"
#include "SlotWidget.h"
#include "ItemIconWidget.h"
#include "ARPG_AKC/BaseItem.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UGridWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	InventoryComponent = PlayerCharacter ? PlayerCharacter->FindComponentByClass<UInventoryComponent>() : nullptr;
}

void UGridWidget::CreateGridBackground(int32 MaxGridSize, int32 MaxRow)
{
	if(bBackgroundGenerated) return;
	GridRows = MaxRow;
	for (int i = 0; i < MaxGridSize; ++i)
	{
		DETERMINE_GRID_LOCATION(i, MaxRow, Row, Column);
		
		SlotWidget = CreateWidget<USlotWidget>(this, SlotWidgetClass);
		SlotWidget->Key = i;
		SlotWidget->SlotType = ESlotType::InventorySlot;
		
		if (IsValid(SlotWidget))
		{
			GridPanel->AddChildToGrid(SlotWidget, Row, Column);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create USlotWidget instance."));
		}
	}

	if (!GridPanel)
	{
		UE_LOG(LogTemp, Error, TEXT("GridPanel is not initialized."));
		return;
	}
	bBackgroundGenerated = true;
}

void UGridWidget::OnItemAdded(int32 Key, ABaseItem* Item)
{
	AddItemToGrid(Key, Item);
}

void UGridWidget::OnItemRemoved(int32 Key)
{
	
}

void UGridWidget::OnItemMoved(int32 OldKey, int32 NewKey)
{
	OnItemRemoved(OldKey);
	ABaseItem* Item = InventoryComponent->GetItemAtKey(OldKey);
	AddItemToGrid(NewKey, Item);
}

void UGridWidget::AddItemToGrid(int32 Key, ABaseItem* ItemInfo)
{
	DETERMINE_GRID_LOCATION(Key, GridRows, Row, Column);
	
	ItemIconWidget = CreateWidget<UItemIconWidget>(this, ItemIconWidgetSubclass);
	
	if (!IsValid(ItemIconWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("ItemIconWidget is null or invalid"));
		return;
	}
	
	if(IsValid(ItemIconWidget) && ItemInfo)
	{
		ItemIconWidget->Item = ItemInfo;
		ItemIconWidget->Key = Key;
		
		ItemIconGridSlot = GridPanel->AddChildToGrid(ItemIconWidget, Row, Column);
		
		if(IsValid(ItemIconGridSlot))
		{
			ItemIconGridSlot->SetLayer(1);
			ItemIconWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Widget not created"));
	}
}