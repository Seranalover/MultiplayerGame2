// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayerGame2 : ModuleRules
{
	public MultiplayerGame2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"EnhancedInput", 
			"GameplayAbilities", "GameplayTasks", "GameplayTags", //GAS
			"UMG", "Slate", "SlateCore", //GUI
			"AIModule", 
			"Niagara", //Niagara粒子特效
			"OnlineSubsystem", "OnlineSubsystemEOS", "OnlineSubsystemUtils", "Networking", "HTTP", "Json" //联网组件
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
