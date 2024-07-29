// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

UCLASS()
class ARPG_AKC_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AProjectileBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* ProjectileMesh;
	
	UPROPERTY(EditDefaultsOnly)
	float Damage = 5.0f;

	UPROPERTY(EditDefaultsOnly)
	float Speed = 150.0f;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 10.0f;

	UPROPERTY(BlueprintReadWrite, Meta=(ExposeOnSpawn=true))
	FGameplayEffectSpecHandle SpecHandle;

protected:
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ETraceTypeQuery> CollisionChannel;

	UPROPERTY(EditDefaultsOnly)
	float LifeTime = 10.f;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	static bool ImplementsACSInterface(const AActor* Actor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
