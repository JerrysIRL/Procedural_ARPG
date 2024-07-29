// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomInitializeInterface.h"
#include "UObject/NoExportTypes.h"
#include "SkillTreeManager.generated.h"

struct FSkillTreeTableRow;
class USkillTreeNodeData;

USTRUCT(Blueprintable)
struct FSkillTreeNode
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkillTreeNodeData* SkillTreeNodeData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> PrerequisiteToNodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> Prerequisites;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Index;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 SkillTreeIndex;
	
	FSkillTreeNode(USkillTreeNodeData* nodeData): SkillTreeNodeData(nodeData)
	{
		PrerequisiteToNodes = TArray<int32>();
		Prerequisites = TArray<int32>();
		Index = -1;
		SkillTreeIndex = -1;
	}

	FSkillTreeNode() : SkillTreeNodeData(nullptr)
	{
		PrerequisiteToNodes = TArray<int32>();
		Prerequisites = TArray<int32>();
		Index = -1;
		SkillTreeIndex = -1;
	}
};

USTRUCT(Blueprintable)
struct FInnerArrayHolder
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FSkillTreeNode> SkillTreeNodes;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillPointAmountChanged, int, SkillPoints);

class USkillTreeNodeData;

UCLASS()
class ARPG_AKC_API USkillTreeManager : public UActorComponent, public ICustomInitializeInterface
{
	GENERATED_BODY()

public:
	/** The data tables in order as your abilities */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UDataTable*> SkillTreeDataTables;
	
	void GenerateTree();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FInnerArrayHolder> SkillTrees;

	UFUNCTION(BlueprintCallable)
	bool TryPurchaseSkill(int32 Index, int32 SkillTreeIndex);

	virtual void CustomInitialize() override;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnSkillPointAmountChanged OnSkillPointChanged;
protected:
	UPROPERTY(BlueprintReadOnly)
	int SkillPoints = 0;

	UFUNCTION()
	void LevelUp();
	
};
