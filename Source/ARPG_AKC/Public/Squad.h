// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "GameFramework/Pawn.h"
#include "Squad.generated.h"

UCLASS()
class ARPG_AKC_API ASquad : public APawn
{
	GENERATED_BODY()

public:
	
	ASquad();


	UPROPERTY(EditDefaultsOnly, Category=Spawning)
	TArray<TSubclassOf<ABaseEnemy>> SmallEnemies;

	UPROPERTY(EditDefaultsOnly, Category=Spawning)
	TArray<TSubclassOf<ABaseEnemy>> MediumEnemies;
	
	UPROPERTY(EditDefaultsOnly, Category = AI)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(BlueprintReadWrite ,EditDefaultsOnly, Category = AI)
	TArray<ABaseEnemy*> SquadMembers;

	UFUNCTION()
	void RemoveSquadMember(ABaseEnemy* EnemyToRemove);
	
	UPROPERTY()
	FVector Center;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	float DistanceFromCenter = 350.f;
	
	UFUNCTION()
	void SpawnSquad();
	
protected:
	virtual void Destroyed() override;
	
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
