	#pragma once

	#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Kismet/BlueprintFunctionLibrary.h"
	#include "AffixLibrary.generated.h"

	class UAffixDataAsset;

	UENUM()
	enum class EElementalType : uint8
	{
		Physical,
		Spell,
		Fire,
		Cold,
		Lightning
	};

	UENUM()
	enum class EAffixType : uint8
	{
		Prefix,
		Suffix
	};

	UENUM()
	enum class EAttributeType : uint8
	{
		Armor,
		Damage,
		Spell,
		Strength,
		Dexterity,
		Intelligence,
		Health,
		Mana,
		Resistance
	};

	UENUM()
	enum class EValueType : uint8
	{
		Percentage,
		Flat
	};

	UENUM()
	enum class ETextType : uint8
	{
		ValueToElementAttributeType,
		ValuePercentageElementAttributeType,
		AddsMinToMaxValueElementAttributeType,
		ValuePercentageIncreasedEleAttributeType,
		ValueToAttribute
	};

	USTRUCT(Blueprintable)
	struct FAffixesInfo 
	{
		GENERATED_BODY()

		FAffixesInfo(): TextType(), ValueType(), MinValue(0), MaxValue(0), AffixValue(0), ElementType(), AffixType(),
		                AttributeType(), bIsElementalType(false)
		{
		}

		UPROPERTY(EditAnywhere)
		FGameplayTag GameplayTag;
		
		UPROPERTY(EditAnywhere)
		ETextType TextType;

		UPROPERTY(EditAnywhere)
		EValueType ValueType;
		
		UPROPERTY(VisibleAnywhere)
		FText Text;
		
		UPROPERTY(EditAnywhere)
		int32 MinValue;

		UPROPERTY(EditAnywhere)
		int32 MaxValue;

		UPROPERTY(VisibleAnywhere)
		int32 AffixValue;
		
		UPROPERTY(EditAnywhere, meta=(EditCondition = "TextType == ETextType::ValueToElementAttributeType || TextType == ETextType::ValuePercentageElementAttributeType || TextType == ETextType::AddsMinToMaxValueElementAttributeType || TextType == ETextType::ValuePercentageIncreasedEleAttributeType"))
		EElementalType ElementType;

		UPROPERTY(EditAnywhere)
		EAffixType AffixType;
		
		UPROPERTY(EditAnywhere)
		EAttributeType AttributeType;

		UPROPERTY(EditAnywhere)
		bool bIsElementalType;
		
		FText ReturnTextTemplate()
		{
			switch (TextType)
			{
			case ETextType::ValueToElementAttributeType:
				return Text = FText::Format(FText::FromString("+{0} to {1} {2}"), AffixValue, GetElementTypeString(), GetAttributeTypeString());
			case ETextType::ValuePercentageElementAttributeType:
				return Text = FText::Format(FText::FromString("+{0}% {1} {2}"), AffixValue, GetElementTypeString(), GetAttributeTypeString());
			case ETextType::AddsMinToMaxValueElementAttributeType:
				return Text = FText::Format(FText::FromString("Adds {0} to {1} {2} {3}"), MinValue, MaxValue, GetElementTypeString(), GetAttributeTypeString());
			case ETextType::ValuePercentageIncreasedEleAttributeType:
				return Text = FText::Format(FText::FromString("+{0}% increased {1} {2}"), AffixValue, GetElementTypeString(), GetAttributeTypeString());
			case ETextType::ValueToAttribute:
				return Text = FText::Format(FText::FromString("+{0} to {1}"), AffixValue, GetAttributeTypeString());
			default:
				return FText::GetEmpty();
			}
		}

		void SetRandomAffixValue()
		{
			AffixValue = FMath::RandRange(MinValue, MaxValue);
		}

		FText GetElementTypeString() const
		{
			switch (ElementType)
			{
			case EElementalType::Physical:
				return FText::FromString("Physical");
			case EElementalType::Spell:
				return FText::FromString("Spell");
			case EElementalType::Fire:
				return FText::FromString("Fire");
			case EElementalType::Cold:
				return FText::FromString("Cold");
			case EElementalType::Lightning:
				return FText::FromString("Lightning");
			default:
				return FText::FromString("Unknown Element Type");
			}
		}

		FText GetAttributeTypeString() const
		{
			switch (AttributeType)
			{
			case EAttributeType::Armor:
				return FText::FromString("Armor");
			case EAttributeType::Damage:
				return FText::FromString("Damage");
			case EAttributeType::Spell:
				return FText::FromString("Spell");
			case EAttributeType::Strength:
				return FText::FromString("Strength");
			case EAttributeType::Dexterity:
				return FText::FromString("Dexterity");
			case EAttributeType::Intelligence:
				return FText::FromString("Intelligence");
			case EAttributeType::Health:
				return FText::FromString("Health");
			case EAttributeType::Mana:
				return FText::FromString("Mana");
			case EAttributeType::Resistance:
				return FText::FromString("Resistance");
			default:
				return FText::FromString("Unknown Attribute Type");
			}
		}
	};

	UCLASS()
	class ARPG_AKC_API UAffixLibrary : public UBlueprintFunctionLibrary
	{
		GENERATED_BODY()

	public:
		UFUNCTION(BlueprintCallable, Category = "Affix Generation")
		static TArray<UAffixDataAsset*> GenerateAffixes(int32 MinAffixes, int32 MaxAffixes, UDataTable* AffixTable);
		
	};