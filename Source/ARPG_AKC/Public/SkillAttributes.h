// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTreeManager.h"
#include "Components/ActorComponent.h"
#include "SkillAttributes.generated.h"


USTRUCT()
struct FSkillAttributesStruct
{
	GENERATED_BODY()

	int ProjectileAmount = 1;
	float CooldownRecoverySpeed = 1;
	int damage = 0;
	int spellDamage = 0;

	FSkillAttributesStruct(){}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARPG_AKC_API USkillAttributes : public UActorComponent, public ICustomInitializeInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillAttributes();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY()
	TArray<FSkillAttributesStruct> SkillAttributes;

	virtual void CustomInitialize() override;
	
protected:
	UPROPERTY()
	USkillTreeManager* SkillTreeManager;

	UFUNCTION()
	void SkillUnlocked(USkillTreeNodeData* SkillTreeNodeData, int32 SkillTreeIndex);
};

