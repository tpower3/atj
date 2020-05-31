// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Atj : ModuleRules
{
	public Atj(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Json", "JsonUtilities"});
    }
}
