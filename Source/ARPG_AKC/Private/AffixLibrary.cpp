// Fill out your copyright notice in the Description page of Project Settings.


#include "AffixLibrary.h"
#include "Algo/RandomShuffle.h"
#include "ARPG_AKC/Affixes.h"

TArray<UAffixDataAsset*> UAffixLibrary::GenerateAffixes(int32 MinAffixes, int32 MaxAffixes, UDataTable* AffixTable)
{
	{
			TArray<UAffixDataAsset*> AffixDataAssets;

			
			if (!AffixTable)
			{
				UE_LOG(LogTemp, Warning, TEXT("AffixTable is not found on Base Item!"));
				return AffixDataAssets;
			}
			
			const FString ContextString(TEXT("Affix Table Context"));
			const TArray<FName> RowNames = AffixTable->GetRowNames();
			
			if (RowNames.Num() == 0) return AffixDataAssets;
			
			const int32 NumAffixes = FMath::RandRange(MinAffixes, MaxAffixes);
			
			AffixDataAssets.Empty();
			
			int32 AffixesAdded = 0;
			int32 AmountPrefixes = FMath::RandRange(0, MinAffixes);
			int32 AmountSuffixes = NumAffixes - AmountPrefixes;
			
			if (AmountSuffixes > MinAffixes)
			{
				AmountPrefixes += AmountSuffixes - MinAffixes;
				AmountSuffixes = MinAffixes; 
			}

			// Check whether the item has rolled max mods
			if(NumAffixes == MaxAffixes) 
			{
				// Set both prefixes and suffixes to be equal to the Minimum amount of Affixes (either 1 or 3)
				// This fixes the issue of rolling a 6 on NumAffixes when it's a rare.
				// The rule is that the item has to have 3 of each mod if it rolls MaxAffixes.
				AmountPrefixes = MinAffixes;
				AmountSuffixes = MinAffixes;
			}
			
			int32 PrefixCount = 0; 
			int32 SuffixCount = 0;
			
			Algo::RandomShuffle(RowNames); // Change this later to a weight system

			TArray<UAffixDataAsset*> PrefixAssets;
			TArray<UAffixDataAsset*> SuffixAssets;

			
			PrefixAssets.Empty();
			SuffixAssets.Empty();
			
			for (const auto RowName : RowNames)
			{
				const FAffixes* AffixRow = AffixTable->FindRow<FAffixes>(RowName, ContextString);

				AffixRow->AffixDataAsset->AffixesInfo.SetRandomAffixValue();
				UAffixDataAsset* DuplicatedAffixDataAsset = DuplicateObject<UAffixDataAsset>(AffixRow->AffixDataAsset, GetTransientPackage(), NAME_None);
				
				if (AffixRow && AffixRow->AffixDataAsset && AffixesAdded < NumAffixes)
				{
					if (AffixRow->AffixDataAsset->AffixesInfo.AffixType == EAffixType::Prefix && PrefixCount < AmountPrefixes)
					{
						if (!PrefixAssets.Contains(AffixRow->AffixDataAsset)) // Check if the prefix is already added to prevent duplication
						{
							PrefixAssets.Add(DuplicatedAffixDataAsset);
							++AffixesAdded;
							++PrefixCount;
						}
					}
					else if (AffixRow->AffixDataAsset->AffixesInfo.AffixType == EAffixType::Suffix && SuffixCount < AmountSuffixes)
					{
						if (!SuffixAssets.Contains(AffixRow->AffixDataAsset)) // Check if the suffix is already added to prevent duplication
						{
							SuffixAssets.Add(DuplicatedAffixDataAsset);
							++AffixesAdded;
							++SuffixCount;
						}
					}
				}
				if (AffixesAdded >= NumAffixes)	break;
			}
			
			AffixDataAssets.Append(PrefixAssets);
			AffixDataAssets.Append(SuffixAssets);
			
			for (UAffixDataAsset* AffixAsset : AffixDataAssets)
			{
				AffixAsset->AffixesInfo.ReturnTextTemplate();
			}
			
			return AffixDataAssets;
		}
}
