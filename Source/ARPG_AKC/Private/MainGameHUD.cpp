// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameHUD.h"
#include "MainHUDWidget.h"
#include "Blueprint/UserWidget.h"


void AMainGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainHudWidgetClass)
	{
		MainHUDWidget = CreateWidget<UMainHUDWidget>(this->GetWorld(), MainHudWidgetClass);
		
		if (IsValid(MainHUDWidget))
		{
			MainHUDWidget->InitializeWidget();
			MainHUDWidget->AddToViewport();
		}
	}
}