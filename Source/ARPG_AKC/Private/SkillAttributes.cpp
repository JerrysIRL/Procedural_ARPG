// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAttributes.h"

#include "MacroHelper.h"
#include "SkillTreeNodeData.h"

// Sets default values for this component's properties
USkillAttributes::USkillAttributes()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USkillAttributes::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USkillAttributes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USkillAttributes::CustomInitialize()
{
	SkillTreeManager = GetOwner()->GetComponentByClass<USkillTreeManager>();

	if (!IsValid(SkillTreeManager))
	{
		DEBUG_PRINT("Skill Tree Manager null");
		return;
	}

	for (const auto SkillTreeNodeArray : SkillTreeManager->SkillTrees)
	{
		SkillAttributes.Add(FSkillAttributesStruct());
		for (const auto SkillTreeNode : SkillTreeNodeArray.SkillTreeNodes)
		{
			SkillTreeNode.SkillTreeNodeData->UnlockedDelegate.AddDynamic(this, &USkillAttributes::SkillUnlocked);
		}
	}
}

void USkillAttributes::SkillUnlocked(USkillTreeNodeData* SkillTreeNodeData, int32 SkillTreeIndex)
{
	for (const auto UpgradeType : SkillTreeNodeData->UpgradeTypes)
	{
		switch (UpgradeType.UpgradeType)
		{
		case ESkillUpgradeType::SpellDamage:
			SkillAttributes[SkillTreeIndex].damage += static_cast<int>(FMath::Floor(UpgradeType.Value));
			break;
		case ESkillUpgradeType::FireDamage:
			SkillAttributes[SkillTreeIndex].damage += static_cast<int>(FMath::Floor(UpgradeType.Value));
			break;
		case ESkillUpgradeType::ProjectileAmount:
			SkillAttributes[SkillTreeIndex].ProjectileAmount += static_cast<int>(FMath::Floor(UpgradeType.Value));
			break;
		case ESkillUpgradeType::CooldownRecoverySpeed:
			SkillAttributes[SkillTreeIndex].CooldownRecoverySpeed -= UpgradeType.Value;
			SkillAttributes[SkillTreeIndex].CooldownRecoverySpeed = FMath::Clamp(SkillAttributes[SkillTreeIndex].CooldownRecoverySpeed, .25f, 1.f);
			break;
		default: ;
		}
	}
}

