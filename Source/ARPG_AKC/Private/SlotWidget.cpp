// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotWidget.h"

#include "EquipmentComponent.h"
#include "GridWidget.h"
#include "InventoryComponent.h"
#include "ItemIconWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void USlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	InventoryComponent = PlayerCharacter ? PlayerCharacter->FindComponentByClass<UInventoryComponent>() : nullptr;
	EquipmentComponent = PlayerCharacter ? PlayerCharacter->FindComponentByClass<UEquipmentComponent>() : nullptr;
	
	if(!IsValid(InventoryComponent))
	{
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Inventory Component not found."));
	}
}

bool USlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	UItemIconWidget* Payload = Cast<UItemIconWidget>(InOperation->Payload);
	
	if(!IsValid(Payload))
	{
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Payload not found for OnDrop function in SlotWidget"));
		return false;
	}
	
	// Checking drop on an Inventory slot
	if(SlotType == ESlotType::InventorySlot)
	{
		if(Payload->Key == Key)
		{
			Payload->SetVisibility(ESlateVisibility::Visible);
			return false;
		}
		
		if(Payload->Key != Key && Payload->CurrentSlotType == ESlotType::InventorySlot)
		{
			if (IsValid(InventoryComponent))
			{
				// Move items around ONLY in inventory slots
				InventoryComponent->OnItemMoved(Payload->Key, Key);
			}
			Payload->RemoveItem();
			GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Added to Inventoryslot"));

			return true;
		}

		// Drop an item inside of inventory FROM equipment slot
		if(Payload->Key != Key && Payload->CurrentSlotType == ESlotType::EquipmentSlot)
		{
			// Add an item to an inventory slot at drop location
			EquipmentComponent->OnItemUnEquipped(Payload->Item);
			InventoryComponent->OnItemAdded(Key, Payload->Item);
			Payload->RemoveItem();
			return true;
		}
	}

	// Equipment slots
	if(SlotType == ESlotType::EquipmentSlot)
	{
		if(Payload->ItemSlotType != ItemSlotType)
		{
			Payload->SetVisibility(ESlateVisibility::Visible);
			return false;
		}
		
		if(Payload->ItemSlotType == ItemSlotType)
		{
			if(IsValid(EquipmentComponent))
			{
				EquipmentComponent->OnItemEquipped(Payload->Item);
				InventoryComponent->OnItemRemoved(Payload->Key);
			}
			Payload->RemoveItem();
			GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Added to equipmentslot"));
			return true;
		}
	}

	Payload->SetVisibility(ESlateVisibility::Visible);
	return false;
}

FReply USlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}