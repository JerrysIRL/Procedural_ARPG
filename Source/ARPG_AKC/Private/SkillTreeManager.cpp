// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeManager.h"
#include "AbilitySystemInterface.h"
#include "MacroHelper.h"
#include "SkillTreeTableRow.h"
#include "ARPG_AKC/PlayerAttributeSet.h"


void USkillTreeManager::GenerateTree()
{
	SkillTrees.Empty();
	
	DEBUG_PRINT(FString::SanitizeFloat(SkillTreeDataTables.Num()))\
	
	for (int i = 0; i < SkillTreeDataTables.Num(); i++)
	{
		if (!IsValid(SkillTreeDataTables[i])) continue;
		SkillTrees.Add(FInnerArrayHolder());
		
		TArray<FSkillTreeTableRow*> Rows;
		SkillTreeDataTables[i]->GetAllRows("", Rows);

		SkillTrees[i].SkillTreeNodes.Empty();
		for (const auto Row : Rows)
		{
			Row->SkillTreeNodeData->Index = i;
			SkillTrees[i].SkillTreeNodes.Add(FSkillTreeNode(DuplicateObject<USkillTreeNodeData>(Row->SkillTreeNodeData, GetTransientPackage())));
		}
	
		for (int j = 0; j < Rows.Num(); ++j)
		{
			FSkillTreeTableRow* NextRow = Rows[j];

			SkillTrees[i].SkillTreeNodes[j].Index = j;
			SkillTrees[i].SkillTreeNodes[j].SkillTreeIndex = i;

			for (const auto Index : NextRow->Prerequisites)
			{
				if (Index >= Rows.Num()) continue;
			
				SkillTrees[i].SkillTreeNodes[Index].PrerequisiteToNodes.Add(j);
				SkillTrees[i].SkillTreeNodes[j].Prerequisites.Add(Index);
			}
		}
	}

	DEBUG_PRINT("Tree Generate")
}

bool USkillTreeManager::TryPurchaseSkill(int32 Index, int32 SkillTreeIndex)
{
	if (SkillPoints <= 0) return false;
	
	FSkillTreeNode& SkillTreeNode = SkillTrees[SkillTreeIndex].SkillTreeNodes[Index];

	for (const auto PrerequisiteIndex : SkillTreeNode.Prerequisites)
	{
		if (!SkillTrees[SkillTreeIndex].SkillTreeNodes[PrerequisiteIndex].SkillTreeNodeData->GetUnlockedValue()) return false;
	}

	SkillTreeNode.SkillTreeNodeData->UnlockSkill();
	SkillPoints--;
	OnSkillPointChanged.Broadcast(SkillPoints);

	return true;
}

void USkillTreeManager::CustomInitialize()
{
	GenerateTree();
	
	if (!GetOwner()->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass())) return;

	UAbilitySystemComponent* ACS = Cast<IAbilitySystemInterface>(GetOwner())->GetAbilitySystemComponent();

	const UPlayerAttributeSet* PlayerAttributes = Cast<UPlayerAttributeSet>(ACS->GetAttributeSet
	(UPlayerAttributeSet::StaticClass()));

	if (!IsValid(PlayerAttributes))
	{
		DEBUG_PRINT("Player Attribute not on ACS");
		return;
	}
	const_cast<FOnPlayerLevelUp&>(PlayerAttributes->PlayerLevelUpDelegate).AddDynamic(this,&USkillTreeManager::LevelUp);
}

void USkillTreeManager::LevelUp()
{
	SkillPoints++;
	OnSkillPointChanged.Broadcast(SkillPoints);
}

