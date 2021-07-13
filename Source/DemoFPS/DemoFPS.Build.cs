// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DemoFPS : ModuleRules
{
	public DemoFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
