// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"


#include "InventoryComponent.h"
#include "ItemInfoWidget.h"
#include "MainGameHUD.h"
#include "MainHUDWidget.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ParticleDataAsset.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("Collider");
	SetRootComponent(BoxComponent);
	BoxComponent->SetSimulatePhysics(true);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshComponent->SetupAttachment(BoxComponent);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("Particle");
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
	Inventory = Cast<UInventoryComponent>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetComponentByClass(UInventoryComponent::StaticClass()));
	BindHoverEvents();
}

void ABaseItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!IsValid(NiagaraComponent)) return;
		// Just for the component to always point upwards.
		NiagaraComponent->SetWorldRotation(FRotator(90, 0, 0));
}

void ABaseItem::GenerateItem(FItemGenericInfo& ItemData)
{
	GenerationInfo = ItemData;
	const float LootUpwardsImpulse = FMath::RandRange(600, 1000);
	
	// Change mesh
	MeshComponent->SetStaticMesh(GenerationInfo.Mesh);

	if(!bAffixesGenerated)
	{
		// Generate Affixes
		AffixDataAssets = UAffixLibrary::GenerateAffixes(
			GenerationInfo.GetMinMaxAffixes().first,
			GenerationInfo.GetMinMaxAffixes().second,
			AffixTable);
	}
	
	GenerateParticleEffect();
	
	// Push it in a random direction
	const FVector RandomDirection = FVector(FMath::VRand()).GetSafeNormal() * LootUpwardsImpulse;
	
	BoxComponent->AddImpulse(RandomDirection);
}

void ABaseItem::GenerateParticleEffect()
{
	if (GenerationInfo.ParticleEffect && IsValid(GenerationInfo.ParticleDataAsset))
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			GenerationInfo.ParticleEffect,
			MeshComponent, 
			FName(TEXT("None")), 
			FVector::ZeroVector,
			FRotator::ZeroRotator, 
			EAttachLocation::KeepRelativeOffset, 
			false 
		);
	}
}

void ABaseItem::BindHoverEvents()
{
	MainGameHUD = Cast<AMainGameHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	ItemInfoWidget = MainGameHUD->MainHUDWidget->ItemInfoWidget;

	BoxComponent->OnBeginCursorOver.AddDynamic(this, &ABaseItem::ShowWidget);
	BoxComponent->OnEndCursorOver.AddDynamic(this, &ABaseItem::HideWidget);
	BoxComponent->OnClicked.AddDynamic(this, &ABaseItem::AddToInventory);
}

void ABaseItem::ShowWidget(UPrimitiveComponent* Component)
{
	ItemInfoWidget->OnItemHovered.Broadcast(GenerationInfo);
	ItemInfoWidget->ItemAffixesDelegate.Broadcast(AffixDataAssets);
	ItemInfoWidget->ItemBackgroundColorDelegate.Broadcast(GenerationInfo.SetColor());
	
}

void ABaseItem::HideWidget(UPrimitiveComponent* Component)
{
	
	ItemInfoWidget->OnItemUnhovered.Broadcast();
}

void ABaseItem::AddToInventory(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (Inventory && !Inventory->IsInventoryFull())
	{
		Inventory->AddItem(this);
		
		//Temporary fix to make the player not move towards the item when getting picked up.
		HideWidget(MeshComponent);
		MeshComponent->SetVisibility(false);
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseItem::DestroyItem, 0.1f, false);
	}
}

void ABaseItem::DestroyItem()
{
	Destroy();
}
