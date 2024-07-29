// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "ARPG_AKC/ARPG_AKCCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;
	auto root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(root);
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	ProjectileMesh->SetupAttachment(root);

	InitialLifeSpan = LifeTime;
}


void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

bool AProjectileBase::ImplementsACSInterface(const AActor* Actor)
{
	return Actor->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass());
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector newPos = GetActorLocation() + GetActorForwardVector() * Speed * DeltaTime;
	FHitResult HitResult;
	
	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(), newPos, Radius, CollisionChannel, false, TArray<AActor*>() = {GetInstigator()}, EDrawDebugTrace::None, HitResult, true))
	{
		//Health logic
		if (ImplementsACSInterface(HitResult.GetActor()))
		{
			auto TargetACS = Cast<IAbilitySystemInterface>(HitResult.GetActor())->GetAbilitySystemComponent();
			TargetACS->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}

		Destroy();
	}
	else
	{
		SetActorLocation(newPos);
	}
}
