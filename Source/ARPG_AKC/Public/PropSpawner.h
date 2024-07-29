// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "PropSpawner.generated.h"

UCLASS()
class ARPG_AKC_API APropSpawner : public AActor
{
	GENERATED_BODY()
public:	
	APropSpawner();
	virtual ~APropSpawner() override;
	
	UFUNCTION(BlueprintCallable)
	static APropSpawner* GetInstance();
	
	UFUNCTION(BlueprintCallable)
	void SpawnProp(UStaticMesh* Mesh, AActor* PropOwner, const FTransform& Transform);

private:

	UPROPERTY()
	TMap<FString, UHierarchicalInstancedStaticMeshComponent*> OwnerMeshToHISMC = TMap<FString, UHierarchicalInstancedStaticMeshComponent*>();

	inline static APropSpawner* Instance = nullptr;

protected:
	virtual void Destroyed() override;
	virtual void BeginPlay() override;
};
