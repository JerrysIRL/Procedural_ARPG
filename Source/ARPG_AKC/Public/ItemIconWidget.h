// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotWidget.h"
#include "ARPG_AKC/BaseItem.h"
#include "Blueprint/UserWidget.h"
#include "ItemIconWidget.generated.h"

class UGridWidget;
class AMainGameHUD;
class UItemInfoWidget;
class UCanvasPanel;
class UImage;
/**
 * 
 */

UCLASS()
class ARPG_AKC_API UItemIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ABaseItem* Item;
		
	UPROPERTY()
	int32 Key;

	void RemoveItem();

	EItemSlotType ItemSlotType;

	ESlotType CurrentSlotType;
protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	UEquipmentComponent* EquipmentComponent;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY()
	AMainGameHUD* MainGameHUD;

	UPROPERTY()
	UItemInfoWidget* ItemInfoWidget;
	
	UPROPERTY()
	UItemInfoWidget* NewItemInfoWidget;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
