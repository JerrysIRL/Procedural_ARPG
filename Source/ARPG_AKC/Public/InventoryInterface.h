// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPG_AKC/BaseItem.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPG_AKC_API IInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void OnItemAdded(int32 Key, ABaseItem* Item) = 0;
	virtual void OnItemRemoved(int32 Key) = 0;
	virtual void OnItemMoved(int32 OldKey, int32 NewKey) = 0;
};
