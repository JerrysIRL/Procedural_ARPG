// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemIconWidget.h"

#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "InventoryWidget.h"
#include "ItemInfoWidget.h"
#include "MainGameHUD.h"
#include "MainHUDWidget.h"
#include "ARPG_AKC/BaseItem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UItemIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainGameHUD = Cast<AMainGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	
	InventoryComponent = InventoryComponent = PlayerCharacter ? PlayerCharacter->FindComponentByClass<UInventoryComponent>() : nullptr;
	EquipmentComponent = PlayerCharacter ? PlayerCharacter->FindComponentByClass<UEquipmentComponent>() : nullptr;
	NewItemInfoWidget = MainGameHUD->MainHUDWidget->InventoryTooltip;
	
	ItemIcon->SetBrushFromTexture(Item->GenerationInfo.Icon);

	ItemSlotType = Item->GenerationInfo.ItemSlotType;
}

void UItemIconWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if (!IsValid(NewItemInfoWidget))
	{
		UE_LOG(LogTemp, Warning, TEXT("No NewItemInfoWidget created"));
		return;
	}

	NewItemInfoWidget->RemoveFromParent();
	NewItemInfoWidget->AddToViewport();
	
	if(!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item"));
		return;
	}
	
	// Broadcast events
	NewItemInfoWidget->OnItemHovered.Broadcast(Item->GenerationInfo);
	NewItemInfoWidget->ItemBackgroundColorDelegate.Broadcast(Item->GenerationInfo.SetColor());
	NewItemInfoWidget->ItemAffixesDelegate.Broadcast(Item->AffixDataAssets);
}

void UItemIconWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (!IsValid(NewItemInfoWidget))
	{
		UE_LOG(LogTemp, Warning, TEXT("No NewItemInfoWidget created"));
		return;
	}
	
	NewItemInfoWidget->OnItemUnhovered.Broadcast();
}

FReply UItemIconWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Hello!!!!!"));
		FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		return Reply.NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UItemIconWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector2D MousePosition = InMouseEvent.GetScreenSpacePosition();
	
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	FVector2D WidgetSize = NewItemInfoWidget->GetDesiredSize();
	
	FVector2D NewPosition = MousePosition;

	// Right side boundary check
    if (NewPosition.X + WidgetSize.X * 1.5 > ViewportSize.X)
    {
        NewPosition.X = ViewportSize.X - WidgetSize.X * 1.5f;
    }

    // Bottom side boundary check
    if (NewPosition.Y + WidgetSize.Y * 1.5f > ViewportSize.Y)
    {
        NewPosition.Y = ViewportSize.Y - WidgetSize.Y * 1.5f;
    }
	
	// Set the new position of the widget
	NewItemInfoWidget->SetPositionInViewport(NewPosition);
    
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UItemIconWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	
	auto DragDrop = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());

	if(IsValid(DragDrop))
	{
		DragDrop->Payload = this;
		DragDrop->DefaultDragVisual = this;
		DragDrop->Pivot = EDragPivot::CenterCenter;
		
		OutOperation = DragDrop;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Not valid!"));
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Dragging!"));
}

bool UItemIconWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UItemIconWidget* Payload = Cast<UItemIconWidget>(InOperation->Payload);

	if(!IsValid(Payload))
	{
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Old Item not found for OnDrop function in ItemIconWidget"));
		return false;
	}

	/*if(Payload->ItemSlotType != ItemSlotType)
	{
		Payload->SetVisibility(ESlateVisibility::Visible);
		return false;
	
	}*/
	
	// Switch items for Equipment to Inventory and vice versa
	if(Payload->CurrentSlotType == ESlotType::EquipmentSlot && this->CurrentSlotType == ESlotType::InventorySlot && Payload->ItemSlotType == ItemSlotType)
	{
		ABaseItem* InvItem = InventoryComponent->GetItemAtKey(Key);
		if(InvItem)
		{
			InventoryComponent->OnItemRemoved(Key);
			InventoryComponent->OnItemAdded(Key, Payload->Item);
			EquipmentComponent->OnItemSwitch(Payload->Item, InvItem);
		}
	}
	// Switch items for Inventory to Equipment and vice versa
	else if(Payload->CurrentSlotType == ESlotType::InventorySlot && this->CurrentSlotType == ESlotType::EquipmentSlot && Payload->ItemSlotType == ItemSlotType)
	{
		ABaseItem* InvItem = InventoryComponent->GetItemAtKey(Payload->Key);
		if(InvItem)
		{
			EquipmentComponent->OnItemSwitch(Item, InvItem);
			InventoryComponent->OnItemRemoved(Payload->Key);
			InventoryComponent->OnItemAdded(Payload->Key, Item);
		}
	}
	// Switch items inside of inventory
	else if(Payload->CurrentSlotType == ESlotType::InventorySlot && this->CurrentSlotType == ESlotType::InventorySlot)
	{
		ABaseItem* InvItem = InventoryComponent->GetItemAtKey(Key);
		if(InvItem)
		{
			InventoryComponent->OnItemRemoved(Key);
			InventoryComponent->OnItemAdded(Key, Payload->Item);
			InventoryComponent->OnItemRemoved(Payload->Key);
			InventoryComponent->OnItemAdded(Payload->Key, InvItem);
		}
	}
	else
	{
		Payload->SetVisibility(ESlateVisibility::Visible);
		return false;
	}
	Payload->RemoveItem();
	RemoveItem();
	return true;
}

void UItemIconWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	auto Payload = Cast<UItemIconWidget>(InOperation->Payload);

	if (!Payload)
	{
		return;
	}
	
	UInventoryWidget* InventoryWidget = MainGameHUD->MainHUDWidget->InventoryWidget;
	UEquipmentWidget* EquipmentWidget = MainGameHUD->MainHUDWidget->InventoryWidget->EquipmentWidget;
	
	FVector2D MousePosition = InDragDropEvent.GetScreenSpacePosition();

	FGeometry InventoryGeometry = InventoryWidget->GetCachedGeometry();
	FGeometry EquipmentGeometry = EquipmentWidget->GetCachedGeometry();

	// Check if the mouse is inside the inventory or equipment widget
	bool bIsInsideInventory = InventoryGeometry.IsUnderLocation(MousePosition);
	bool bIsInsideEquipment = EquipmentGeometry.IsUnderLocation(MousePosition);

	// If the item was dragged from an inventory slot and is not inside the inventory or equipment, remove it
	if (Payload->CurrentSlotType == ESlotType::InventorySlot && !bIsInsideInventory && !bIsInsideEquipment)
	{
		InventoryComponent->DropItem(Payload->Item);
		InventoryComponent->OnItemRemoved(Payload->Key);
		Payload->RemoveItem();
	}
	// If the item was dragged from an equipment slot and is not inside the inventory or equipment, remove it
	else if (Payload->CurrentSlotType == ESlotType::EquipmentSlot && !bIsInsideInventory && !bIsInsideEquipment)
	{
		EquipmentComponent->DropItem(Payload->Item);
		EquipmentComponent->OnItemUnEquipped(Payload->Item);
		Payload->RemoveItem();
	}
}

void UItemIconWidget::RemoveItem()
{
	RemoveFromParent();
}
