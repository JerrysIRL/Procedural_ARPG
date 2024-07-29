// Fill out your copyright notice in the Description page of Project Settings.


#include "..\Public\PropSpawner.h"

APropSpawner::APropSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

APropSpawner::~APropSpawner()
{
	if (Instance == this)
	{
		Instance = nullptr;
	}
}

void APropSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (Instance == nullptr)
	{
		Instance = this;
	}
}

void APropSpawner::Destroyed()
{
	if (Instance == this)
	{
		Instance = nullptr;
	}
	Super::Destroyed();
}

APropSpawner* APropSpawner::GetInstance()
{
	return Instance;
}

void APropSpawner::SpawnProp(UStaticMesh* Mesh, AActor* Tile, const FTransform& Transform)
{
	const FString OwnerID = FString::FromInt(Tile->GetUniqueID());
	const FString MeshID = Mesh->GetPathName();
	const FString CompoundKey = OwnerID + "_" + MeshID;
	UHierarchicalInstancedStaticMeshComponent* MeshComponent = OwnerMeshToHISMC.FindRef(CompoundKey);
	if (!MeshComponent)
	{
		MeshComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(Tile);
		MeshComponent->SetStaticMesh(Mesh);
		MeshComponent->SetMobility(EComponentMobility::Static);
		MeshComponent->AttachToComponent(Tile->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		MeshComponent->bDisableCollision = true;
		MeshComponent->RegisterComponent();
		MeshComponent->SetCanEverAffectNavigation(false);
		OwnerMeshToHISMC.Add(CompoundKey, MeshComponent);
	}

	MeshComponent->AddInstance(Transform);
}


