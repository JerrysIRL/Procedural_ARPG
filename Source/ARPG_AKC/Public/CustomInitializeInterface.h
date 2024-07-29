// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CustomInitializeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCustomInitializeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPG_AKC_API ICustomInitializeInterface 
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void CustomInitialize() = 0;
};
