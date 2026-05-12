// Copyright 2026 Luka Markuš. All rights reserved.

using UnrealBuildTool;

public class ZombiesEditor : ModuleRules
{
	public ZombiesEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"Zombies",
			"StateTreeModule",
			"StateTreeEditorModule",
			"UnrealEd",
			"Slate",
			"SlateCore",
			"EditorFramework",
			"ToolMenus",
			"GameplayTags",
			"InputCore",
			"GameplayTagsEditor",
			"EditorWidgets",
			"Settings",
			"UMG",
			"UMGEditor",
			"ScriptableEditorWidgets",
			"EditorScriptingUtilities",
			"Blutility",
			"PropertyEditor",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ZombiesEditor",
		});
	}
}

