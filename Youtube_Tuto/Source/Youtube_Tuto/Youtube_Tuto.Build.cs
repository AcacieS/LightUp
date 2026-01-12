// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Youtube_Tuto : ModuleRules
{
	public Youtube_Tuto(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GeometryCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Youtube_Tuto",
			"Youtube_Tuto/Variant_Platforming",
			"Youtube_Tuto/Variant_Platforming/Animation",
			"Youtube_Tuto/Variant_Combat",
			"Youtube_Tuto/Variant_Combat/AI",
			"Youtube_Tuto/Variant_Combat/Animation",
			"Youtube_Tuto/Variant_Combat/Gameplay",
			"Youtube_Tuto/Variant_Combat/Interfaces",
			"Youtube_Tuto/Variant_Combat/UI",
			"Youtube_Tuto/Variant_SideScrolling",
			"Youtube_Tuto/Variant_SideScrolling/AI",
			"Youtube_Tuto/Variant_SideScrolling/Gameplay",
			"Youtube_Tuto/Variant_SideScrolling/Interfaces",
			"Youtube_Tuto/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
