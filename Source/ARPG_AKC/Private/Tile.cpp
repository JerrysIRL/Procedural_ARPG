// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

#include "PropSpawner.h"

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->SetMobility(EComponentMobility::Static);
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetMobility(EComponentMobility::Static);
	SideMaterials.SetNum(8);
	StaticMesh->SetCollisionProfileName(FName("Ground"));
}

float ATile::GetMultiplier(const TSubclassOf<ATile> Tile) const
{
	for (auto NeighborMultiplier : NeighborMultipliers)
	{
		if (NeighborMultiplier.TileClass == Tile)
			return NeighborMultiplier.Multiplier;
	}
	return 1.0f;
}

float ATile::ApplyUniqueWeightChanges_Implementation(UCell* OwnerCell, float Weight, int32 GridSize)
{
	return Weight;
}

void ATile::BeginPlay()
{
	Super::BeginPlay();
}

FTransform ATile::GenerateTransform(FVector2D BoxOrigin, FVector2D BoxExtent, float ScaleMin, float ScaleMax) const
{
    float RandomX = FMath::FRandRange(BoxOrigin.X - BoxExtent.X, BoxOrigin.X + BoxExtent.X);
    float RandomY = FMath::FRandRange(BoxOrigin.Y - BoxExtent.Y, BoxOrigin.Y + BoxExtent.Y);
    float ActorLocationZ = GetActorLocation().Z;
    FVector StartPosition(RandomX + GetActorLocation().X, RandomY + GetActorLocation().Y, ActorLocationZ + 200.0f);
    FVector EndPosition(RandomX + GetActorLocation().X, RandomY + GetActorLocation().Y, ActorLocationZ - 200.0f);

    FHitResult HitResult;

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPosition, EndPosition, ECC_GameTraceChannel4);
	
    FVector Position;
    FVector SurfaceNormal;
    if (bHit)
    {
        Position = HitResult.Location;
        SurfaceNormal = HitResult.Normal;
    }
    else
    {
        Position = FVector(RandomX + GetActorLocation().X, RandomY + GetActorLocation().Y, StaticMesh->GetRelativeLocation().Z);
        SurfaceNormal = FVector::UpVector;
    }

    FRotator Rotation = FRotationMatrix::MakeFromZX(SurfaceNormal, FVector::ForwardVector).Rotator();
    Rotation.Yaw = FMath::FRandRange(0.f, 359.f);

    float ScaleFactor = FMath::FRandRange(ScaleMin, ScaleMax);
    FVector Scale(ScaleFactor, ScaleFactor, ScaleFactor);

    return FTransform(Rotation.Quaternion(), Position, Scale);
}


bool ATile::ShouldTickIfViewportsOnly() const
{
	for (const auto SpawnArea : FoliageSpawnAreas)
	{
		DrawDebugBox(GetWorld(), GetActorLocation() + FVector(SpawnArea.Origin, 0), FVector(SpawnArea.Extent.X, SpawnArea.Extent.Y, 100), FColor::Red, false, -1, 0, 5);
	}
	for (const auto SpawnArea : PropSpawnAreas)
	{
		DrawDebugBox(GetWorld(), GetActorLocation() + FVector(SpawnArea.Origin, 0), FVector(SpawnArea.Extent.X, SpawnArea.Extent.Y, 100), FColor::Green, false, -1, 0, 5);
	}
	return true;
}

void ATile::CalculateRotatedBox(FVector2D& BoxOrigin, FVector2D& BoxExtent) const
{
	const FRotator ActorRotation = GetActorRotation();
	
	if (FMath::IsNearlyEqual(ActorRotation.Yaw, 90, 0.1f))
	{
		BoxOrigin = FVector2D(-BoxOrigin.Y, BoxOrigin.X);
		BoxExtent = FVector2D(BoxExtent.Y, BoxExtent.X);
	}

	else if (FMath::IsNearlyEqual(ActorRotation.Yaw, -90, 0.1f))
	{
		BoxOrigin = FVector2D(BoxOrigin.Y, -BoxOrigin.X);
		BoxExtent = FVector2D(BoxExtent.Y, BoxExtent.X);
	}

	else if (FMath::IsNearlyEqual(FMath::Abs(ActorRotation.Yaw), 180, 0.1f))
	{
		BoxOrigin = FVector2D(-BoxOrigin.X, -BoxOrigin.Y);
	}
}

void ATile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Initialize();
}

void ATile::Initialize()
{
	for (auto& PropSpawnArea : PropSpawnAreas)
	{
		CalculateRotatedBox(PropSpawnArea.Origin, PropSpawnArea.Extent);
	}
	
	for (auto& FoliageSpawnArea : FoliageSpawnAreas)
	{
		CalculateRotatedBox(FoliageSpawnArea.Origin, FoliageSpawnArea.Extent);
	}
	SpawnProp();
	SpawnFoliage();
}

void ATile::SpawnProp()
{
	if (PercentChanceToSpawnProp == 0 || Props.Num() == 0 || PropSpawnAreas.Num() == 0) return;

	if (FMath::RandRange(1, 100) > PercentChanceToSpawnProp) return;

	const FSpawnArea RandomSpawnArea = PropSpawnAreas[FMath::RandRange(0, PropSpawnAreas.Num() - 1)];
	const FVector2D PropSpawnRangeOrigin = RandomSpawnArea.Origin;
	const FVector2D PropSpawnRangeExtent = RandomSpawnArea.Extent;

	
    const FTransform Transform = GenerateTransform(PropSpawnRangeOrigin, PropSpawnRangeExtent, PropScaleMin, PropScaleMax);
	
	float TotalWeight = 0.0f;
	for (const FMeshAndWeight MeshAndWeight : Props)
	{
		TotalWeight += MeshAndWeight.Weight;
	}

	const float RandomWeight = FMath::FRandRange(0, TotalWeight);
	float CurrentWeight = 0;

	float SelectedIndex = 0;
	for (; SelectedIndex < Props.Num(); ++SelectedIndex)
	{
		CurrentWeight += Props[SelectedIndex].Weight;
		if (CurrentWeight >= RandomWeight) break;
	}
	UStaticMesh* Mesh = Props[SelectedIndex].Mesh;

	const auto Prop = NewObject<UStaticMeshComponent>(this);
	Prop->SetStaticMesh(Mesh);
	Prop->SetMobility(EComponentMobility::Static);
	Prop->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	Prop->SetWorldTransform(Transform);
	Prop->RegisterComponent();
}

void ATile::SpawnFoliage()
{
	if (AmountOfFoliage == 0 || Foliage.Num() == 0 || FoliageSpawnAreas.Num() == 0 || !IsValid(APropSpawner::GetInstance())) return;
	APropSpawner* PropSpawner = APropSpawner::GetInstance();
	for (int32 i = 0; i < AmountOfFoliage; ++i)
	{
		const FSpawnArea RandomSpawnArea = FoliageSpawnAreas[FMath::RandRange(0, PropSpawnAreas.Num() - 1)];
		const FVector2D FoliageSpawnRangeOrigin = RandomSpawnArea.Origin;
		const FVector2D FoliageSpawnRangeExtent = RandomSpawnArea.Extent;

		const FTransform Transform = GenerateTransform(FoliageSpawnRangeOrigin, FoliageSpawnRangeExtent, FoliageScaleMin, FoliageScaleMax);
		UStaticMesh* Mesh = Foliage[FMath::RandRange(0, Foliage.Num() - 1)];
		PropSpawner->SpawnProp(Mesh, this, Transform);
	}
}
