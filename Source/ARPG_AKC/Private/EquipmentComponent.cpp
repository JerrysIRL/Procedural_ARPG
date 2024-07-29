// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"

#include "AbilitySystemInterface.h"
#include "InventoryWidget.h"
#include "MacroHelper.h"
#include "MainGameHUD.h"
#include "MainHUDWidget.h"
#include "ARPG_AKC/ARPG_AKCCharacter.h"
#include "ARPG_AKC/PlayerAttributeSet.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadMesh->SetVisibility(false);
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetVisibility(false);

	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	ChestMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChestMesh->SetVisibility(false);

	HandsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HandsMesh->SetVisibility(false);
	
	FeetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FeetMesh"));
	FeetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FeetMesh->SetVisibility(false);
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AARPG_AKCCharacter>(GetOwner());
	
	AMainGameHUD* MainGameHUD = Cast<AMainGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());

	MainGameHUD->MainHUDWidget->InventoryWidget->EquipmentWidget->CreateSlots();

	RegisterObserver(MainGameHUD->MainHUDWidget->InventoryWidget->EquipmentWidget);

	if (!GetOwner()->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
	{
		DEBUG_PRINT("Owner does not implement AbilitySystemInterface")
		return;
	}

	OwnerACS = Cast<IAbilitySystemInterface>(GetOwner())->GetAbilitySystemComponent();
}

void UEquipmentComponent::EquipItem(ABaseItem* Item)
{
	ABaseItem* NewItem = DuplicateObject<ABaseItem>(Item, GetTransientPackage(), NAME_None);
	
	OnItemEquipped(NewItem);
}

bool UEquipmentComponent::ContainsItemType(ABaseItem* NewItem)
{
	for (auto Item : Items)
	{
		if(Item->GenerationInfo.ItemSlotType == NewItem->GenerationInfo.ItemSlotType)
		{
			return true;
		}
	}
	return false;
}

void UEquipmentComponent::OnItemEquipped(ABaseItem* Item)
{
	if(ContainsItemType(Item)) return;
	GivePlayerAffixStats(Item);
	NotifyItemEquipped(Item);
	Items.Add(Item);
	EquipMesh(Item);
}

void UEquipmentComponent::OnItemUnEquipped(ABaseItem* Item)
{
	RemoveMesh(Item);
	NotifyItemUnEquipped(Item);
	RemoveAffixStatsFromPlayer(Item);
	Items.Remove(Item);
}

void UEquipmentComponent::OnItemSwitch(ABaseItem* OldItem, ABaseItem* NewItem)
{
	NotifyItemSwitch(OldItem, NewItem);
	OnItemUnEquipped(OldItem);
	OnItemEquipped(NewItem);
}

void UEquipmentComponent::DropItem(ABaseItem* Item)
{
	if(Items.Contains(Item))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Item Dropped"));
		auto ForwardVector = GetOwner()->GetActorForwardVector();
		auto SpawnLocation = GetOwner()->GetActorLocation() + ForwardVector * 100;
		
		auto NewItem = GetWorld()->SpawnActor<ABaseItem>(Item->GetClass(), SpawnLocation, FRotator::ZeroRotator);
		NewItem->bAffixesGenerated = true;
		NewItem->GenerateItem(Item->GenerationInfo);
		NewItem->AffixDataAssets = Item->AffixDataAssets;
	}
}

ABaseItem* UEquipmentComponent::GetItemAtSlot(EItemSlotType SlotType)
{
	for (auto Item : Items)
	{
		if(Item->GenerationInfo.ItemSlotType == SlotType)
		{
			return Item;
		}
	}
	return nullptr;
}

void UEquipmentComponent::UnEquipItem(ABaseItem* Item)
{
	OnItemUnEquipped(Item);
}

void UEquipmentComponent::RegisterObserver(IEquipmentInterface* Observer)
{
	Observers.Add(Observer);
}

void UEquipmentComponent::UnregisterObservers()
{
	Observers.Empty();
}

void UEquipmentComponent::GivePlayerAffixStats(ABaseItem* EquippedItem) const
{
	GenerateEffectFromGenerationInfo(EquippedItem->GenerationInfo);
	
	for (const auto AffixDataAsset : EquippedItem->AffixDataAssets)
	{
		FAffixesInfo& AffixInfo = AffixDataAsset->AffixesInfo;

		// Create a dynamic instant Gameplay Effect to give the bounties
		UGameplayEffect* AttributeChangeEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AttributeChange")));
		AttributeChangeEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		int32 Idx = AttributeChangeEffect->Modifiers.Num();
		AttributeChangeEffect->Modifiers.SetNum(Idx + 1);

		FGameplayModifierInfo& GameplayModifierInfo = AttributeChangeEffect->Modifiers[Idx];
		GameplayModifierInfo.ModifierMagnitude = FScalableFloat(AffixInfo.AffixValue);
		GameplayModifierInfo.ModifierOp = EGameplayModOp::Additive;

		FGameplayAttribute Attribute = GetAttributeFromAffixInfo(AffixInfo);
		if (!Attribute.IsValid()) continue;
		
		GameplayModifierInfo.Attribute = Attribute;
	

		OwnerACS->ApplyGameplayEffectToSelf(AttributeChangeEffect, 1.0f, OwnerACS->MakeEffectContext());
	}
}

void UEquipmentComponent::RemoveAffixStatsFromPlayer(ABaseItem* UnequippedItem) const
{
	GenerateEffectFromGenerationInfo(UnequippedItem->GenerationInfo, false);
	
	for (const auto AffixDataAsset : UnequippedItem->AffixDataAssets)
	{
		FAffixesInfo& AffixInfo = AffixDataAsset->AffixesInfo;

		// Create a dynamic instant Gameplay Effect to give the bounties
		UGameplayEffect* AttributeChangeEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AttributeChange")));
		AttributeChangeEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

		int32 Idx = AttributeChangeEffect->Modifiers.Num();
		AttributeChangeEffect->Modifiers.SetNum(Idx + 1);

		FGameplayModifierInfo& GameplayModifierInfo = AttributeChangeEffect->Modifiers[Idx];
		GameplayModifierInfo.ModifierMagnitude = FScalableFloat(-AffixInfo.AffixValue);
		GameplayModifierInfo.ModifierOp = EGameplayModOp::Additive;

		FGameplayAttribute Attribute = GetAttributeFromAffixInfo(AffixInfo);
		if (!Attribute.IsValid()) continue;
		
		GameplayModifierInfo.Attribute = Attribute;
	

		OwnerACS->ApplyGameplayEffectToSelf(AttributeChangeEffect, 1.0f, OwnerACS->MakeEffectContext());
	}
}

void UEquipmentComponent::GenerateEffectFromGenerationInfo(const FItemGenericInfo& GenerationInfo, bool bEquipped) const
{
	UGameplayEffect* ArmorAttributeChange = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("ArmorAttributeChange")));
	ArmorAttributeChange->DurationPolicy = EGameplayEffectDurationType::Instant;

	int32 Idx = ArmorAttributeChange->Modifiers.Num();
	ArmorAttributeChange->Modifiers.SetNum(Idx + 1);

	FGameplayModifierInfo& GameplayModifierInfo = ArmorAttributeChange->Modifiers[Idx];
	const float ArmorValue = GenerationInfo.ItemStats.ArmorValue;
	GameplayModifierInfo.ModifierMagnitude = FScalableFloat(bEquipped ? ArmorValue : -ArmorValue);
	GameplayModifierInfo.ModifierOp = EGameplayModOp::Additive;
	GameplayModifierInfo.Attribute = UPlayerAttributeSet::GetArmorAttribute();

	OwnerACS->ApplyGameplayEffectToSelf(ArmorAttributeChange, 1.0f, OwnerACS->MakeEffectContext());
}

FGameplayAttribute UEquipmentComponent::GetAttributeFromAffixInfo(const FAffixesInfo& AffixInfo)
{
	if (AffixInfo.bIsElementalType)
	{
		switch (AffixInfo.ValueType) {
		case EValueType::Percentage:
			switch (AffixInfo.ElementType) {
				
		case EElementalType::Physical:
			return UPlayerAttributeSet::GetPercentagePhysicalDamageAttribute();
			
		case EElementalType::Spell:
			return UPlayerAttributeSet::GetPercentageSpellDamageAttribute();
			
		case EElementalType::Fire:
			return UPlayerAttributeSet::GetPercentageFireDamageAttribute();
			
		case EElementalType::Cold:
			return UPlayerAttributeSet::GetPercentagePhysicalDamageAttribute();
			
		case EElementalType::Lightning:
			return UPlayerAttributeSet::GetPercentageLightningDamageAttribute();
			
			}	
			break;
		case EValueType::Flat:
			switch (AffixInfo.ElementType) {
				
		case EElementalType::Physical:
			return UPlayerAttributeSet::GetFlatPhysicalDamageAttribute();
			
		case EElementalType::Spell:
			return UPlayerAttributeSet::GetFlatSpellDamageAttribute();

		case EElementalType::Fire:
			return UPlayerAttributeSet::GetFlatFireDamageAttribute();
			
		case EElementalType::Cold:
			return UPlayerAttributeSet::GetFlatPhysicalDamageAttribute();
			
		case EElementalType::Lightning:
			return UPlayerAttributeSet::GetFlatLightningDamageAttribute();
			
			}
		}
	}
	else
	{
		switch (AffixInfo.AttributeType) {
		case EAttributeType::Armor:
			return UPlayerAttributeSet::GetArmorAttribute();
		case EAttributeType::Damage:
			break;
		case EAttributeType::Spell:
			break;
		case EAttributeType::Strength:
			break;
		case EAttributeType::Dexterity:
			break;
		case EAttributeType::Intelligence:
			break;
		case EAttributeType::Health:
			return UPlayerAttributeSet::GetMaxHealthAttribute();
			break;
		case EAttributeType::Mana:
			return UPlayerAttributeSet::GetMaxManaAttribute();
			break;
		case EAttributeType::Resistance:
			break;
		}
	}

	return NULL;
}


void UEquipmentComponent::EquipMesh(ABaseItem* Item)
{
	switch (Item->GenerationInfo.ItemSlotType)
	{
	case EItemSlotType::WeaponSlot:
		WeaponMesh->SetStaticMesh(Item->GetMeshComponent()->GetStaticMesh());
		WeaponMesh->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Item_R"));
		WeaponMesh->SetVisibility(true);
		OnGearEquipChangeDelegate.Broadcast(WeaponMesh, EItemSlotType::WeaponSlot);
		break;
	case EItemSlotType::GlovesSlot:
		//HandsMesh->SetStaticMesh(Item->GetMeshComponent()->GetStaticMesh());
		//HandsMesh->SetVisibility(true);
		//HandsMesh->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("headSocket"));;
		break;
	case EItemSlotType::HelmetSlot:
		HeadMesh->SetStaticMesh(Item->GetMeshComponent()->GetStaticMesh());
		HeadMesh->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("headSocket"));;
		HeadMesh->SetVisibility(true);
		OnGearEquipChangeDelegate.Broadcast(HeadMesh, EItemSlotType::HelmetSlot);
		break;
	case EItemSlotType::ChestSlot:
		//ChestMesh->SetStaticMesh(Item->GetMeshComponent()->GetStaticMesh());
		//ChestMesh->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("chestSocket"));
		//ChestMesh->SetVisibility(true);
		//OnGearEquipChangeDelegate.Broadcast(ChestMesh, EItemSlotType::ChestSlot);
		break;
	case EItemSlotType::BootsSlot:
		//FeetMesh->SetStaticMesh(Item->GetMeshComponent()->GetStaticMesh());
		//FeetMesh->SetVisibility(true);
		//FeetMesh->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("feetSocket"));;
		break;
	}
}

void UEquipmentComponent::RemoveMesh(ABaseItem* Item)
{
	switch (Item->GenerationInfo.ItemSlotType)
	{
	case EItemSlotType::WeaponSlot:
		WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		WeaponMesh->SetVisibility(false);
		OnGearUnEquipChangeDelegate.Broadcast(Item->GetMeshComponent(), EItemSlotType::WeaponSlot);
		break;
	case EItemSlotType::GlovesSlot:
		//HandsMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		break;
	case EItemSlotType::HelmetSlot:
		HeadMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		HeadMesh->SetVisibility(false);
		OnGearUnEquipChangeDelegate.Broadcast(Item->GetMeshComponent(), EItemSlotType::HelmetSlot);
		break;
	case EItemSlotType::ChestSlot:
		ChestMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		ChestMesh->SetVisibility(false);
		OnGearUnEquipChangeDelegate.Broadcast(Item->GetMeshComponent(), EItemSlotType::ChestSlot);
		break;
	case EItemSlotType::BootsSlot:
		//FeetMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		break;
	}
}
