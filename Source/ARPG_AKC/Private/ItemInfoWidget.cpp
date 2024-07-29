// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInfoWidget.h"
#include "AffixDataAsset.h"
#include "ARPG_AKC/BaseItem.h"
#include "Fonts/SlateFontInfo.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"


void UItemInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(IsValid(this))
		SetVisibility(ESlateVisibility::Hidden);
}

void UItemInfoWidget::InitializeWidget()
{
	OnItemHovered.AddDynamic(this, &UItemInfoWidget::UpdateItemInfo);
	if (OnItemHovered.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnItemHovered Event is bound"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnItemHovered Event not binded"));
	}
	OnItemUnhovered.AddDynamic(this, &UItemInfoWidget::HideWidget);
    if (OnItemUnhovered.IsBound())
    {
    	UE_LOG(LogTemp, Warning, TEXT("OnItemUnhovered Event is bound"));
    }
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnItemUnhovered Event not binded"));
	}
	ItemAffixesDelegate.AddDynamic(this, &UItemInfoWidget::UpdateAffixInfo);
	if (ItemAffixesDelegate.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemAffixesDelegate Event is bound"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemAffixesDelegate Event not binded"));
	}
	ItemBackgroundColorDelegate.AddDynamic(this, &UItemInfoWidget::UpdateWidgetColor);
	if (ItemBackgroundColorDelegate.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemBackgroundColorDelegate Event is bound"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemBackgroundColorDelegate Event not binded"));
	}
}		

void UItemInfoWidget::UpdateWidgetColor(FColor Color)
{
	BackgroundImage->SetBrushTintColor(Color);
}

void UItemInfoWidget::UpdateItemInfo(const FItemGenericInfo& ItemInfo) 
{
	
	if (ItemName)
	{
		ItemName->SetText(ItemInfo.ItemTextData.ItemName);
	}
	
	if (ItemImage)
	{
		ItemImage->SetBrushFromTexture(ItemInfo.Icon);
	}
	
	if(ItemDescription)
	{
		ItemDescription->SetText(ItemInfo.ItemTextData.ItemDescription);
	}

	FString DamageString;
	FString ArmorString;
	FString ConsumableString;
	
	if(ItemStats)
	{
		switch (ItemInfo.ItemStats.ItemType)
		{
		case EItemType::Weapon:
			DamageString = FString::Printf(TEXT("Damage: %.2f"), ItemInfo.ItemStats.DamageValue);
			ItemStats->SetText(FText::FromString(DamageString));
			break;
		case EItemType::Armor:
			ArmorString = FString::Printf(TEXT("Armor: %.2f"), ItemInfo.ItemStats.ArmorValue);
			ItemStats->SetText(FText::FromString(ArmorString));
			break;
		case EItemType::Consumable:
			ConsumableString = FString::Printf(TEXT("Amount Healed: %.2f"), ItemInfo.ItemStats.ConsumableValue); // To change "Healed" with an ENUM later
			ItemStats->SetText(FText::FromString(ConsumableString));
			break;
		default:
			break;
		}
	}

	ShowWidget();
}


void UItemInfoWidget::UpdateAffixInfo(TArray<UAffixDataAsset*>& ItemDataAssets)
{
	if(AffixesBox)
	{
		AffixesBox->ClearChildren();

		for (UAffixDataAsset* ItemDataAsset : ItemDataAssets)
		{
			UTextBlock* AffixTextBlock = NewObject<UTextBlock>();
			if (AffixTextBlock && IsValid(ItemDataAsset))
			{
				AffixTextBlock->SetText(ItemDataAsset->AffixesInfo.Text); // Line 124
				AffixTextBlock->SetColorAndOpacity(FColor::Black);
				FontInfo = AffixTextBlock->GetFont();
				FontInfo.Size = 14;
				AffixTextBlock->SetFont(FontInfo);
				AffixTextBlock->SetJustification(ETextJustify::Center);
				AffixesBox->AddChild(AffixTextBlock);
			}
		}
	}
}

void UItemInfoWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UItemInfoWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
}