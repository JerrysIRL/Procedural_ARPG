// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MacroHelper.h"
#include "Squad.h"
#include "GameFramework/GameModeBase.h"
#include "ARPG_AKCGameMode.generated.h"

class UEnemyDeathManager;

UCLASS(minimalapi)
class AARPG_AKCGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	UFUNCTION()
	void Print(ABaseEnemy* BaseEnemy)
	{
		DEBUG_PRINT(BaseEnemy->GetActorNameOrLabel());
	};

public:
	AARPG_AKCGameMode();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASquad> SquadBlueprint;

};



