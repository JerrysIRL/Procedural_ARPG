#include "InventoryComponent.h"

#include "GridWidget.h"
#include "InventoryInterface.h"
#include "InventoryWidget.h"
#include "MainGameHUD.h"
#include "MainHUDWidget.h"
#include "ARPG_AKC/BaseItem.h"
#include "Kismet/GameplayStatics.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	AMainGameHUD* MainGameHUD = Cast<AMainGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	
	// Register the observers once on begin play
	if(IsValid(MainGameHUD))
	{
		UGridWidget* GridWidget = MainGameHUD->MainHUDWidget->InventoryWidget->GridWidget;
		
		GridWidget->CreateGridBackground(MaxGridSize, MaxRow);
		
		RegisterObserver(GridWidget);
	}
}

void UInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UnregisterObservers();
}

void UInventoryComponent::AddItem(ABaseItem* Item)
{
	//Check if the inventory slots are full first and if an Item exists (just in case)
	if(IsInventoryFull() || !Item) return;
	
	//Duplicate the item, since the item on the floor gets destroyed and memory address is not valid anymore.
	ABaseItem* NewItem = DuplicateObject<ABaseItem>(Item, GetTransientPackage(), NAME_None);
	
	if(!IsValid(NewItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("NewItem was not created"));
	}

	// Check the grid for an empty slot.
	for(int32 Index = 0; Index < MaxGridSize; ++Index)
	{
		// If there's an empty slot at Index, add the item there.
		if(!IsSlotFull(Index))
		{
			OnItemAdded(Index, NewItem);
			break; 
		}
	}
}

void UInventoryComponent::OnItemAdded(const int32 SlotKey, ABaseItem* Item)
{
	NotifyItemAdded(SlotKey, Item);
	InventoryMap.Add(SlotKey, Item);
}

void UInventoryComponent::OnItemRemoved(int32 Key)
{
	NotifyItemRemoved(Key);
	InventoryMap.Remove(Key);
}

void UInventoryComponent::DropItem(ABaseItem* Item)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Item Dropped"));
	auto ForwardVector = GetOwner()->GetActorForwardVector();
	auto SpawnLocation = GetOwner()->GetActorLocation() + ForwardVector * 100;
		
	auto NewItem = GetWorld()->SpawnActor<ABaseItem>(Item->GetClass(), SpawnLocation, FRotator::ZeroRotator);
	NewItem->bAffixesGenerated = true;
	NewItem->GenerateItem(Item->GenerationInfo);
	NewItem->AffixDataAssets = Item->AffixDataAssets;
	
}

bool UInventoryComponent::IsSlotFull(int32 Key)
{
    return InventoryMap.Find(Key) != nullptr;
}

bool UInventoryComponent::IsInventoryFull()
{
	for (int32 Index = 0; Index < MaxGridSize; ++Index)
	{
		if (!IsSlotFull(Index))
		{
			return false;
		}
	}
	return true;
}

void UInventoryComponent::RegisterObserver(IInventoryInterface* Observer)
{
	Observers.Add(Observer);
}

void UInventoryComponent::UnregisterObservers()
{
	Observers.Empty();
}

void UInventoryComponent::OnItemMoved(int32 OldKey, int32 NewKey)
{
	NotifyItemMoved(OldKey, NewKey);
	if(InventoryMap.Contains(OldKey))
	{
		ABaseItem* Item = InventoryMap[OldKey];
		InventoryMap.Remove(OldKey);
		InventoryMap.Add(NewKey, Item);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryMap doesn't contain old key."));
	}
}

ABaseItem* UInventoryComponent::GetItemAtKey(int32 Key)
{
	if(InventoryMap.Contains(Key))
	{
		ABaseItem* Item = InventoryMap[Key];
		return Item;
	}
	return nullptr;
}