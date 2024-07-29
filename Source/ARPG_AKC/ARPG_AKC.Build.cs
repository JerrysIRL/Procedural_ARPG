// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ARPG_AKC : ModuleRules
{
	public ARPG_AKC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core", 
	        "CoreUObject", 
	        "Engine", 
	        "InputCore", 
	        "NavigationSystem", 
	        "AIModule", 
	        "Niagara", 
	        "EnhancedInput", 
	        "ProceduralMeshComponent",
	        "NavigationSystem",
	        "GameplayTasks",
			"UMG",
			"Slate",
			"SlateCore", 
			"GameplayAbilities"
        });
        
        PrivateDependencyModuleNames.AddRange(new string[] {"GameplayTags", "GameplayTasks"});
    }
}
