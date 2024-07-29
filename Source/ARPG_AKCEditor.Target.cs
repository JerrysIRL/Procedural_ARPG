// Copyright Epic Games, Inc. All Rights Reserved.

// hej

using UnrealBuildTool;
using System.Collections.Generic;

public class ARPG_AKCEditorTarget : TargetRules
{
	public ARPG_AKCEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("ARPG_AKC");
	}
}
