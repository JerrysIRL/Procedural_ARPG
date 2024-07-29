// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ARPG_AKC/Interfaces/EquipmentInterface.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class AARPG_AKCCharacter;
class ABaseItem;
struct FGameplayTagContainer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGearEquipChangeDelegate, UStaticMeshComponent*, EquippedItemMesh, EItemSlotType, SlotType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGearUnEquipChangeDelegate, UStaticMeshComponent*, EquippedItemMesh, EItemSlotType, SlotType);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARPG_AKC_API UEquipmentComponent : public UActorComponent, public IEquipmentInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();
	
	void EquipItem(ABaseItem* Item);

	bool ContainsItemType(ABaseItem* NewItem);

	virtual void OnItemEquipped(ABaseItem* Item) override;
	virtual void OnItemUnEquipped(ABaseItem* Item) override;
	virtual void OnItemSwitch(ABaseItem* OldItem, ABaseItem* NewItem) override;

	void DropItem(ABaseItem* Item);
	
	ABaseItem* GetItemAtSlot(EItemSlotType SlotType);

	FOnGearEquipChangeDelegate OnGearEquipChangeDelegate;
	FOnGearUnEquipChangeDelegate OnGearUnEquipChangeDelegate;
	
	FORCEINLINE TArray<ABaseItem*> GetAllEquippedItems()
	{
		return Items;
	}

	// GETTER Meshes for each bone slot
	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	UStaticMeshComponent* GetHeadMesh() const { return HeadMesh; }
	UStaticMeshComponent* GetChestMesh() const { return ChestMesh; }
	UStaticMeshComponent* GetFeetMesh() const { return FeetMesh; }
	UStaticMeshComponent* GetHandsMesh() const { return HandsMesh; }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	FGameplayTagContainer* EquipmentTags;

	UPROPERTY(EditAnywhere)
	TArray<ABaseItem*> Items;
	
	void UnEquipItem(ABaseItem* Item);

	UPROPERTY()
	UAbilitySystemComponent* OwnerACS;
	
private:

	UPROPERTY()
	AARPG_AKCCharacter* OwnerCharacter;
	
	TSet<IEquipmentInterface*> Observers;

	void RegisterObserver(IEquipmentInterface* Observer);
	
	void UnregisterObservers();

	FORCEINLINE void NotifyItemEquipped(ABaseItem* Item)
	{
		for (IEquipmentInterface* Observer : Observers)
		{
			Observer->OnItemEquipped(Item);
		}
	}

	FORCEINLINE void NotifyItemUnEquipped(ABaseItem* Item)
	{
		for (IEquipmentInterface* Observer : Observers)
		{
			Observer->OnItemUnEquipped(Item);
		}
	}

	FORCEINLINE void NotifyItemSwitch(ABaseItem* OldItem, ABaseItem* NewItem)
	{
		for (IEquipmentInterface* Observer : Observers)
		{
			Observer->OnItemSwitch(OldItem, NewItem);
		}
	}

	void GivePlayerAffixStats(ABaseItem* EquippedItem) const;
	void RemoveAffixStatsFromPlayer(ABaseItem* UnequippedItem) const;
	void GenerateEffectFromGenerationInfo(const FItemGenericInfo& GenerationInfo, bool bEquipped = true) const;
	static FGameplayAttribute GetAttributeFromAffixInfo(const FAffixesInfo& AffixInfo);
	
	void EquipMesh(ABaseItem* Item);
	void RemoveMesh(ABaseItem* Item);
	// Meshes for each bone slot
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HeadMesh;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ChestMesh;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* FeetMesh;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HandsMesh;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WeaponMesh;
};
