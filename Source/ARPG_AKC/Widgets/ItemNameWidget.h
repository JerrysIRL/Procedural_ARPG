// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ItemNameWidget.generated.h"

class ABaseItem;
class UCanvasPanel;
class AMainGameHUD;
/**
 * 
 */
UCLASS()
class ARPG_AKC_API UItemNameWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
public:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* BackgroundColor;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY()
	ABaseItem* Item;

private:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	AMainGameHUD* MainGameHUD;
};
