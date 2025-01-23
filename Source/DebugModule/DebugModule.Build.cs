// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DebugModule : ModuleRules
{
	public DebugModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicIncludePaths.Add(ModuleDirectory);
		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "EnhancedInput" });
	}
}
