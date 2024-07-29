// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Squad.h"
#include "Tickable.h"
#include "ARPG_AKC/ARPG_AKCGameMode.h"
#include "Subsystems/WorldSubsystem.h"
#include "SquadManager.generated.h"


UCLASS(NotBlueprintable, meta=(BlueprintSpawnableComponent))
class ARPG_AKC_API USquadManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	float MinSpawnRadius = 2500.f;

	float MaxSpawnRadius = 3000.f;

	float DespawningRadius = 2500.f;

	float DistanceBetweenSquads = 1200.f;
	
	int MaxActiveSquads = 3;

	TArray<FString> Levels = {"SergeiTestLevel", "WorldGenerationTest",};
	
	float GetSpawnRadius() const;
	
	UPROPERTY()
	ACharacter* Player = nullptr;

	UPROPERTY()
	AARPG_AKCGameMode* GameMode = nullptr;

	UPROPERTY(Transient)
	TArray<ASquad*> Squads;

	UFUNCTION()
	void HandleSquadDespawning();
	
	UFUNCTION()
	void HandleSquadSpawning();
	
	FVector GetValidSquadPosition();

	bool IsValidLocation(FVector Location);
	
	FTimerHandle DespawnerHandle;

	FTimerHandle SpawnerHandle;
};
