// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentWidget.h"
#include "GridWidget.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */

UCLASS()
class ARPG_AKC_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGridWidget* GridWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEquipmentWidget* EquipmentWidget;
};
