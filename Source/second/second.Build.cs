// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class second : ModuleRules
{
	public second(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { "second" });
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"NavigationSystem", "AIModule", "NetCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate", "SlateCore", "UMG"
		});

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
