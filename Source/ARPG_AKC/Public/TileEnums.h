#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMaterialTypes : uint8
{
	ForestFloor UMETA(DisplayName = "Forest Floor"),
	MossyGround UMETA(DisplayName = "Mossy Ground"),
	RippledSand UMETA(DisplayName = "Rippled Sand"),
	RippledSandWater UMETA(DisplayName = "Rippled Sand Water"),
	Beach UMETA(DisplayName = "Beach"),
	Ocean UMETA(DisplayName = "Ocean")
};

UENUM(BlueprintType)
enum class ETileVariations : uint8
{
	None UMETA(DisplayName = "No additional variations"),
	Rotate90 UMETA(DisplayName = "90° rotation"),
	RotateAll UMETA(DisplayName = "90°, 180°, and 270° rotations"),
};