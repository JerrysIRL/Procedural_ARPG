// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "SkillTreeNodeData.h"
#include "SkillTreeTableRow.generated.h"

USTRUCT(BlueprintType)
struct FSkillTreeTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	USkillTreeNodeData* SkillTreeNodeData;

	UPROPERTY(EditAnywhere)
	TArray<int32> Prerequisites;
};