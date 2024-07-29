#pragma once

#include "CoreMinimal.h"
#include "ParticleDataAsset.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataLibrary.generated.h"

class UNiagaraComponent;
class UParticleDataAsset;

UENUM()
enum class EItemSlotType : uint8
{
	WeaponSlot,
	NecklaceSlot,
	RingSlot,
	GlovesSlot,
	
	HelmetSlot,
	ChestSlot,
	BeltSlot,
	BootsSlot,
	
	TotalCount
};

UENUM()
enum class EItemRarity : uint8
{
	Common = 100 UMETA(DisplayName = "Common") ,
	Uncommon = 75 UMETA(DisplayName = "Uncommon") ,
	Rare = 20 UMETA(DisplayName = "Rare") ,
	Unique = 10 UMETA(DisplayName = "Unique") 
};

UENUM()
enum class EItemType : uint8
{
	Gold UMETA(DisplayName = "Gold"), 
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Accessory UMETA(DisplayName = "Accessory"),
	Consumable UMETA(DisplayName = "Consumable")
};

UENUM()
enum class EColorType : uint8
{
	White,
	Blue,
	Yellow,
	Orange
};

USTRUCT()
struct FItemTextData
{
	GENERATED_BODY()

	FItemTextData()
	{
	}
	
	UPROPERTY(EditAnywhere)
	FText ItemName;

	UPROPERTY(EditAnywhere)
	FText ItemDescription;

	UPROPERTY(EditAnywhere)
	FText InteractionText;
};

USTRUCT()
struct FItemStats
{
	GENERATED_BODY()

	FItemStats(): DamageValue(0), ArmorValue(0), ConsumableValue(0), ItemType(EItemType::Weapon)
	{
	}

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType == EItemType::Weapon"))
	float DamageValue;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType == EItemType::Armor"))
	float ArmorValue;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType == EItemType::Consumable"))
	float ConsumableValue;

	UPROPERTY(EditAnywhere)
	EItemType ItemType;
	
};

USTRUCT()
struct FItemNumericData
{
	GENERATED_BODY()

	FItemNumericData(): MaxStackSize(0), bIsStackable(false)
	{
	}

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsStackable", ClampMin = 0, ClampMax = 500))
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere)
	bool bIsStackable;
};

USTRUCT(BlueprintType)
struct FItemGenericInfo
{
	GENERATED_BODY()

	FItemGenericInfo(): ItemSlotType(), ItemRarity(EItemRarity::Common), ColorType(EColorType::Blue), Icon(nullptr),
	                    Mesh(nullptr)
	{
	}

	UPROPERTY(EditAnywhere)
	EItemSlotType ItemSlotType;
	
	UPROPERTY(EditAnywhere)
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere)
	EColorType ColorType;

	UPROPERTY(EditAnywhere)
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere)
	FItemStats ItemStats;
	
	UPROPERTY(EditAnywhere)
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere)
	UParticleDataAsset* ParticleDataAsset;
	
	UPROPERTY()
	UNiagaraSystem* ParticleEffect;
	
	FORCEINLINE EItemRarity GenerateRandomRarity(int32& NextItem)
	{
		const float TotalWeight = static_cast<int8>(EItemRarity::Unique) +
									static_cast<int8>(EItemRarity::Rare) +
									static_cast<int8>(EItemRarity::Uncommon) +
									static_cast<int8>(EItemRarity::Common);

		const float RandomWeight = FMath::FRandRange(1, TotalWeight);

		UE_LOG(LogTemp, Warning, TEXT("RandomWeight: %f"), RandomWeight);

		if (RandomWeight <= static_cast<int8>(EItemRarity::Unique))
		{
			ColorType = EColorType::Orange;
			NextItem += 5;
			return EItemRarity::Unique;
		}
		
		if (RandomWeight <= static_cast<int8>(EItemRarity::Unique) + static_cast<int8>(EItemRarity::Rare))
		{
			ColorType = EColorType::Yellow;
			NextItem += 3;
			return EItemRarity::Rare;
		}
		
		if (RandomWeight <= static_cast<int8>(EItemRarity::Unique) + static_cast<int8>(EItemRarity::Rare) +
		static_cast<int8>(EItemRarity::Uncommon))
		{
			ColorType = EColorType::Blue;
			NextItem += 2;
			return EItemRarity::Uncommon;
		}
		
		ColorType = EColorType::White;
		NextItem++;
		return EItemRarity::Common;
	}

	FORCEINLINE FColor SetColor() const
	{
		switch (ColorType)
		{
		case EColorType::White:
			return FColor::White;
		case EColorType::Blue:
			return FColor::Cyan;
		case EColorType::Yellow:
			return FColor::Yellow;
		case EColorType::Orange:
			return FColor::Orange;
		default:
			return FColor::White;
		}
	}

	FORCEINLINE UNiagaraSystem* SetParticleEffect()
	{
		switch (ItemRarity)
		{
		case EItemRarity::Common:
			return ParticleEffect = ParticleDataAsset->CommonParticleEffect;
		case EItemRarity::Uncommon:
			return ParticleEffect = ParticleDataAsset->UncommonParticleEffect;
		case EItemRarity::Rare:
			return ParticleEffect = ParticleDataAsset->RareParticleEffect;
		case EItemRarity::Unique:
			return ParticleEffect = ParticleDataAsset->UniqueParticleEffect;
		}
		return ParticleEffect;
	}

	FORCEINLINE std::pair<int32, int32> GetMinMaxAffixes() const
	{
		int32 Min = 0;
		int32 Max = 0;
		
		switch (ItemRarity)
		{
		case EItemRarity::Common:
			break;
		case EItemRarity::Uncommon:
			Min = 1;
			Max = 2;
			break;
		case EItemRarity::Rare:
			Min = 3;
			Max = 6;
			break;
		case EItemRarity::Unique:
			break;
		}
		return std::make_pair(Min, Max);
	}
};

UCLASS()
class UDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Item Generation")
	static FItemGenericInfo GenerateItem(FItemGenericInfo& GenerationInfo, int32& NextItem)
	{
		FItemGenericInfo NewItemData = GenerationInfo;
		NewItemData.ItemRarity = NewItemData.GenerateRandomRarity(NextItem);
		NewItemData.SetParticleEffect();
		
		return NewItemData;
	}
};
