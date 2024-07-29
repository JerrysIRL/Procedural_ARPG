// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class ABaseItem;
class UItemNameWidget;
class UHorizontalBox;
class UInventoryWidget;
class UItemInfoWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoveredItem, ABaseItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUnhovered);
/**
 * 
 */
UCLASS()
class ARPG_AKC_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeWidget();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UItemInfoWidget* ItemInfoWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* InventoryVerticalBox;

	UPROPERTY()
	UItemInfoWidget* InventoryTooltip;

	UFUNCTION()
	void AddItemNameWidget(ABaseItem* Item);

	void RemoveItemNameWidget(ABaseItem* Item);
	
	UFUNCTION()
	void ShowItemInfoWidget(ABaseItem* Item);

	UFUNCTION()
	void HideItemInfoWidget();
	
protected:
	
	virtual void NativeConstruct() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY()
	TMap<ABaseItem*, UItemNameWidget*> ItemNameWidgetMap;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemNameWidget> ItemNameWidgetClass;


	
};
