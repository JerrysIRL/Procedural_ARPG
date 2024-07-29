// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyEquipmentCharacterActor.generated.h"

class UEquipmentComponent;
class USpringArmComponent;

UCLASS()
class ARPG_AKC_API ADummyEquipmentCharacterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADummyEquipmentCharacterActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGearEquipChange(UStaticMeshComponent* EquippedItemMesh, EItemSlotType SlotType);

	UFUNCTION()
	void OnGearUnequipChange(UStaticMeshComponent* EquippedItemMesh, EItemSlotType SlotType);
	
	UPROPERTY(EditAnywhere, Category = "CapturingComponents")
	USceneCaptureComponent2D* USceneCaptureComponent;

	UPROPERTY(EditAnywhere, Category = "CapturingComponents")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, Category = "SkeletalMesh")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "StaticMesh")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "StaticMesh")
	UStaticMeshComponent* HeadMesh;

	UPROPERTY(EditAnywhere, Category = "StaticMesh")
	UStaticMeshComponent* ChestMesh;
	
	UEquipmentComponent* EquipmentComponent; 
};
