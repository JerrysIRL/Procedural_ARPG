// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPG_AKC/BaseItem.h"
#include "UObject/Interface.h"
#include "EquipmentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPG_AKC_API IEquipmentInterface
{
	GENERATED_BODY()

public:

	virtual void OnItemEquipped(ABaseItem* Item) = 0;
	virtual void OnItemUnEquipped(ABaseItem* Item) = 0;
	virtual void OnItemSwitch(ABaseItem* OldItem, ABaseItem* NewItem) = 0;
};
