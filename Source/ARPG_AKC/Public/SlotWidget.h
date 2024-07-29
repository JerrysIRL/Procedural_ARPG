// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPG_AKC/DataLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "SlotWidget.generated.h"

class UItemIconWidget;
/**
 * 
 */
UENUM()
enum class ESlotType : int8
{
	InventorySlot,
	EquipmentSlot
};

class UEquipmentComponent;
class UInventoryComponent;
class AMainGameHUD;
class UGridWidget;

UCLASS()
class ARPG_AKC_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	int32 Key = 0;

	EItemSlotType ItemSlotType;

	ESlotType SlotType;

	UPROPERTY()
	UItemIconWidget* OccupiedItem;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* CanvasPanel;
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Border;
	
	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	UEquipmentComponent* EquipmentComponent;
	
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

};
