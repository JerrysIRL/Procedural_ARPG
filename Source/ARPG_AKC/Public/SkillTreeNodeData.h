// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillTreeNodeData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSkillUnlockedChangeDelegate, USkillTreeNodeData*, SkillTreeNodeData, int32, Index);
class UTexture2D;

UENUM(BlueprintType)
enum class ESkillUpgradeType : uint8
{
	SpellDamage UMETA(DisplayName = "Spell Damage"),
	FireDamage UMETA(DisplayName = "Fire Damage"),
	ProjectileAmount UMETA(DisplayName = "Projectile Amount"),
	CooldownRecoverySpeed UMETA(DisplayName = "Cooldown Recovery Speed")
};

USTRUCT(Blueprintable)
struct FSkillUpgrade
{
	GENERATED_BODY()

	FSkillUpgrade(): UpgradeType(), Value(0)
	{
	}

	UPROPERTY(EditAnywhere)
	ESkillUpgradeType UpgradeType;
	
	UPROPERTY(EditAnywhere)
	float Value;
};

/**
 * 
 */
UCLASS()
class ARPG_AKC_API USkillTreeNodeData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FSkillUpgrade> UpgradeTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image;

	int32 Index;

	FSkillUnlockedChangeDelegate UnlockedDelegate;

	UFUNCTION(BlueprintCallable)
	bool GetUnlockedValue() const
	{
		return bUnlocked;
	}

	void UnlockSkill()
	{
		bUnlocked = true;
		UnlockedDelegate.Broadcast(this, Index);
	}

private:
	UPROPERTY(EditAnywhere)
	bool bUnlocked = false;
};
