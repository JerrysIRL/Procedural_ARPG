#pragma once

#include "CoreMinimal.h"
#include "EquipmentWidget.h"
#include "InventoryInterface.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class IInventoryInterface;
class AMainGameHUD;
class UInventoryWidget;
class ABaseItem;
class USlotWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARPG_AKC_API UInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	void AddItem(ABaseItem* Item);
	bool IsInventoryFull();
	
	virtual void OnItemMoved(int32 OldKey, int32 NewKey) override;
	virtual void OnItemAdded(int32 SlotKey, ABaseItem* Item) override;
	virtual void OnItemRemoved(int32 Key) override;

	void DropItem(ABaseItem* Item);
	
	ABaseItem* GetItemAtKey(int32 Key);

	bool IsSlotFull(int32 Key);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	int32 MaxGridSize = 60;
	int32 MaxRow = 12;
	
	

private:

	UPROPERTY(EditAnywhere)
	TMap<int32, ABaseItem*> InventoryMap;
	
	TSet<IInventoryInterface*> Observers;
	
	void RegisterObserver(IInventoryInterface* Observer);
	
	void UnregisterObservers();
	
//////////////////////////////////////
///////// NOTIFYING OBSERVER FUNCTIONS
	FORCEINLINE void NotifyItemAdded(int32 Key, ABaseItem* Item)
	{
		for (IInventoryInterface* Observer : Observers)
		{
			Observer->OnItemAdded(Key, Item);
		}
	}
	
	FORCEINLINE void NotifyItemRemoved(int32 Key)
	{
		for (IInventoryInterface* Observer : Observers)
		{
			Observer->OnItemRemoved(Key);
		}
	}
	
	FORCEINLINE void NotifyItemMoved(int32 OldKey, int32 NewKey)
	{
		for (IInventoryInterface* Observer : Observers)
		{
			Observer->OnItemMoved(OldKey, NewKey);
		}
	}
/////////////////////////////////////
};
