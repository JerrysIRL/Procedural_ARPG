// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "ParticleDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_AKC_API UParticleDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* CommonParticleEffect;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* UncommonParticleEffect;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* RareParticleEffect;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* UniqueParticleEffect;
};
