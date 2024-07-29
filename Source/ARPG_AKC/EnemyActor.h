// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyActor.generated.h"

class ALevelTileManager;
class ABaseItem;
UCLASS()
class ARPG_AKC_API AEnemyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AEnemyActor();

	float timer = 0;
	virtual void Tick(float DeltaTime) override;
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere)
	bool bGenerate = false;
	
	UPROPERTY()
	ALevelTileManager* LevelScriptActor; 
	
};
