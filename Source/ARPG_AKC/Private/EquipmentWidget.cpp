// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentWidget.h"
#include "SlotWidget.h"
#include "ARPG_AKC/BaseItem.h"
#include "ItemIconWidget.h"
#include "ARPG_AKC/DataLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"


void UEquipmentWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEquipmentWidget::CreateSlots()
{
	AmountOfSlots = static_cast<int8>(EItemSlotType::TotalCount) - 1;
	
	for (int i = 0; i < AmountOfSlots + 1; ++i)
	{
		SlotWidget = CreateWidget<USlotWidget>(this, SlotWidgetClass);

		SlotWidget->ItemSlotType = static_cast<EItemSlotType>(i);
		SlotWidget->SlotType = ESlotType::EquipmentSlot;
		SlotWidget->Key = -1;

		SlotWidgets.Add(SlotWidget);

		UTextBlock* SlotNameText = NewObject<UTextBlock>(); // Create the text widget
		FString SlotNameString = GetEnumName(SlotWidget->ItemSlotType);
		SlotNameText->SetText(FText::FromString(SlotNameString)); // Set the text content
		SlotNameText->SetVisibility(ESlateVisibility::Visible);

		
		if(VerticalBox->GetChildrenCount() < 8)
		{
			UVerticalBoxSlot* TextSlot = VerticalBox->AddChildToVerticalBox(SlotNameText);
			UVerticalBoxSlot* VerticalBoxSlot = VerticalBox->AddChildToVerticalBox(SlotWidget);
			
			// Set Padding and Size for first vertical box
			if(SlotWidget && VerticalBox && TextSlot)
			{
				//Padding for Slots
				FSlateChildSize Size = VerticalBoxSlot->GetSize();
				Size.SizeRule = ESlateSizeRule::Fill;
				VerticalBoxSlot->SetSize(Size);
				
				FMargin CurrentPadding = SlotWidget->GetPadding();
				CurrentPadding.Top = 10.f; 
				CurrentPadding.Bottom = 10.f; 
				CurrentPadding.Left = 10.f;
				SlotWidget->SetPadding(CurrentPadding);

				//Text
				FSlateChildSize TextSize;
				TextSize.SizeRule = ESlateSizeRule::Automatic;
				TextSlot->SetSize(TextSize);

				//Font
				FSlateFontInfo FontInfo;
				FontInfo = SlotNameText->GetFont();
				FontInfo.Size = 14;
				SlotNameText->SetFont(FontInfo);
				SlotNameText->SetJustification(ETextJustify::Center);

				//Padding for Text
				FMargin TextPadding(5.0f, 5.0f, 0, 5.0f); // Adjust as needed
				TextSlot->SetPadding(TextPadding);
			}
		}
		else
		{
			UVerticalBoxSlot* TextSlot = VerticalBox2->AddChildToVerticalBox(SlotNameText);
			UVerticalBoxSlot* VerticalBoxSlot2 = VerticalBox2->AddChildToVerticalBox(SlotWidget);
			
			// Set Padding and Size for second vertical box
			if(SlotWidget && VerticalBox && TextSlot)
			{
				//Padding for Slots
				FSlateChildSize Size = VerticalBoxSlot2->GetSize();
				Size.SizeRule = ESlateSizeRule::Fill;
				VerticalBoxSlot2->SetSize(Size);
				
				FMargin CurrentPadding = SlotWidget->GetPadding();
				CurrentPadding.Top = 10.f; 
				CurrentPadding.Bottom = 10.f; 
				CurrentPadding.Right = 10.f;
				SlotWidget->SetPadding(CurrentPadding);
				
				//Text
				FSlateChildSize TextSize;
				TextSize.SizeRule = ESlateSizeRule::Automatic;
				TextSlot->SetSize(TextSize);

				//Font
				FSlateFontInfo FontInfo;
				FontInfo = SlotNameText->GetFont();
				FontInfo.Size = 14;
				SlotNameText->SetFont(FontInfo);
				SlotNameText->SetJustification(ETextJustify::Center);

				//Padding for Text
				FMargin TextPadding(0, 5.0f, 5.0f, 5.0f);
				TextSlot->SetPadding(TextPadding);
			}
		}
	}
}

void UEquipmentWidget::AddItemToSlot(EItemSlotType SlotType, ABaseItem* Item)
{
	USlotWidget* TargetSlot = FindSlotWidget(SlotType);

	if (!TargetSlot)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Cannot find slot widget"));
		return;
	}
	
	ItemIconWidget = CreateWidget<UItemIconWidget>(this, ItemIconWidgetClass);

	if(!IsValid(ItemIconWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("ItemIconWidget is null or invalid"));
		return;
	}
	
	ItemIconWidget->Item = Item;
	ItemIconWidget->Key = -1;
	ItemIconWidget->CurrentSlotType = SlotWidget->SlotType;
	ItemIconWidget->SetVisibility(ESlateVisibility::Visible);

	auto CanvasSlot = TargetSlot->CanvasPanel->AddChildToCanvas(ItemIconWidget);

	if(CanvasSlot)
	{
		FVector2D IconSize(75, 75);
		CanvasSlot->SetSize(IconSize);
	}
}

void UEquipmentWidget::OnItemEquipped(ABaseItem* Item)
{
	AddItemToSlot(Item->GenerationInfo.ItemSlotType, Item);
}

void UEquipmentWidget::OnItemUnEquipped(ABaseItem* Item)
{
	
}

void UEquipmentWidget::OnItemSwitch(ABaseItem* OldItem, ABaseItem* NewItem)
{
	
}

FString UEquipmentWidget::GetEnumName(EItemSlotType SlotType)
{
	const UEnum* Enum = StaticEnum<EItemSlotType>();

	if (Enum)
	{
		FString EnumName = Enum->GetNameStringByValue(static_cast<int64>(SlotType));

		// Remove the last four characters
		return EnumName.LeftChop(4);
	}
	return FString("Unknown");
}

USlotWidget* UEquipmentWidget::FindSlotWidget(EItemSlotType SlotType)
{
	for (USlotWidget* Widget : SlotWidgets)
	{
		if (Widget->ItemSlotType == SlotType)
		{
			return Widget;
		}
	}
	return nullptr; // Return nullptr if no slot matches the given type
}

bool UEquipmentWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	UItemIconWidget* Payload = Cast<UItemIconWidget>(InOperation->Payload);
	
	if(!IsValid(Payload))
	{
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Yellow, TEXT("Payload not found for OnDrop function in EquipmentWidget"));
		return false;
	}
	
	Payload->SetVisibility(ESlateVisibility::Visible);
	
	return false;
}
