// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainGameHUD.generated.h"

class UMainHUDWidget;
class UItemInfoWidget;
/**
 * 
 */

UCLASS()
class ARPG_AKC_API AMainGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	// Override BeginPlay
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMainHUDWidget* MainHUDWidget;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainHUDWidget> MainHudWidgetClass;

	
};

