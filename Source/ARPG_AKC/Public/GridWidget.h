// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryInterface.h"
#include "Blueprint/UserWidget.h"
#include "GridWidget.generated.h"

class UGridSlot;
class UItemIconWidget;
class ABaseItem;
class USlotWidget;
class UGridPanel;
/**
 * 
 */

UCLASS()
class ARPG_AKC_API UGridWidget : public UUserWidget, public IInventoryInterface
{
	GENERATED_BODY()

#define DETERMINE_GRID_LOCATION(ChildCount, MaxPerRow, Row, Column) \
{ \
	Row = FMath::Floor(static_cast<float>(ChildCount) / MaxPerRow); \
	Column = ChildCount % MaxPerRow; \
} \
	
public:
	
	UPROPERTY(EditAnywhere)
	int32 GridSize;

	void CreateGridBackground(int32 MaxGridSize, int32 MaxRow);
	
	UFUNCTION()
	void AddItemToGrid(int32 Key, ABaseItem* ItemInfo);

	virtual void OnItemMoved(int32 OldKey, int32 NewKey) override;
protected:
	virtual void NativeConstruct() override;
	
	virtual void OnItemAdded(int32 Key, ABaseItem* Item) override;
	virtual void OnItemRemoved(int32 Key) override;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Classes")
	TSubclassOf<USlotWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget Classes")
	TSubclassOf<UItemIconWidget> ItemIconWidgetSubclass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USlotWidget* SlotWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemIconWidget* ItemIconWidget;

	UPROPERTY()
	UGridSlot* ItemIconGridSlot;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGridPanel* GridPanel;

	int32 GridRows;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;
private:
	
	int32 Row = 0;
	
	int32 Column = 0;

	bool bBackgroundGenerated = false;
};
