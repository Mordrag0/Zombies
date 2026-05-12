// Copyright 2026 Luka Markuš. All rights reserved.

using UnrealBuildTool;

public class Zombies : ModuleRules
{
	public Zombies(ReadOnlyTargetRules Target) : base(Target)
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
			"GameplayTags",
			"NetCore",
			"PhysicsCore",
			"NavigationSystem",
			"CommonUI", 
			"Mover", 
			"ChaosMover",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Zombies",
			//"Zombies/Variant_Horror",
			//"Zombies/Variant_Horror/UI",
			//"Zombies/Variant_Shooter",
			//"Zombies/Variant_Shooter/AI",
			//"Zombies/Variant_Shooter/UI",
			//"Zombies/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		PublicDefinitions.Add("UE_WITH_PUSH_MODEL=1");

		SetupIrisSupport(Target);
	}
}

