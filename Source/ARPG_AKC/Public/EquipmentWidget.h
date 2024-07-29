// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPG_AKC/Interfaces/EquipmentInterface.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentWidget.generated.h"


class UItemIconWidget;
enum class EItemSlotType : uint8;
class ABaseItem;
class UVerticalBox;
class USlotWidget;
/**
 * 
 */

UCLASS()
class ARPG_AKC_API UEquipmentWidget : public UUserWidget, public IEquipmentInterface
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void CreateSlots();

	void AddItemToSlot(EItemSlotType SlotType, ABaseItem* Item);
	
	virtual void OnItemEquipped(ABaseItem* Item) override;
	virtual void OnItemUnEquipped(ABaseItem* Item) override;
	virtual void OnItemSwitch(ABaseItem* OldItem, ABaseItem* NewItem) override;

protected:

	FString GetEnumName(EItemSlotType SlotType);
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* VerticalBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* VerticalBox2;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Classes")
	TSubclassOf<USlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Classes")
	TSubclassOf<UItemIconWidget> ItemIconWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USlotWidget* SlotWidget;

	UPROPERTY()
	UItemIconWidget* ItemIconWidget;

	int8 AmountOfSlots;
	
	USlotWidget* FindSlotWidget(EItemSlotType SlotType);

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	TArray<USlotWidget*> SlotWidgets;
};
