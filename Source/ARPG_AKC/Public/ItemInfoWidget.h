// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPG_AKC/DataLibrary.h"
#include "Blueprint/UserWidget.h"
#include "ItemInfoWidget.generated.h"


class UAffixDataAsset;
class UItemDataAsset;
class UVerticalBox;
class UTextBlock;
class UImage;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemHoveredDelegate, const FItemGenericInfo&, ItemInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAffixesDelegate, TArray<UAffixDataAsset*>&, ItemDataAssets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemBackgroundColorDelegate, FColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FItemUnhoveredDelegate);

UCLASS()
class ARPG_AKC_API UItemInfoWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FItemHoveredDelegate OnItemHovered;

	UPROPERTY(BlueprintAssignable)
	FItemUnhoveredDelegate OnItemUnhovered;
	
	UPROPERTY(BlueprintAssignable)
	FItemAffixesDelegate ItemAffixesDelegate;

	UPROPERTY(BlueprintAssignable)
	FItemBackgroundColorDelegate ItemBackgroundColorDelegate;
	
	void InitializeWidget();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* BackgroundImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ItemDescription;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* AffixesBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ItemStats;

	FSlateFontInfo FontInfo;
	
	void ShowWidget();

	UFUNCTION()
	void HideWidget();
	
	UFUNCTION()
	void UpdateAffixInfo(TArray<UAffixDataAsset*>& ItemDataAssets);
	
	UFUNCTION()
	void UpdateItemInfo(const FItemGenericInfo& ItemInfo);

	UFUNCTION()
	void UpdateWidgetColor(FColor Color);
};
