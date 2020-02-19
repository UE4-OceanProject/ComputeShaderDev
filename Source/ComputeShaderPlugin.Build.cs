// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ComputeShaderPlugin : ModuleRules
{
	public ComputeShaderPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		//Our PrivatePCH that we want to globally #include
		PrivatePCHHeaderFile = "Private/PrivatePCH.h";

        // Make sure UBT reminds us of how to keep the project IWYU compliant
        bEnforceIWYU = true;

        //Enable IWYU but keep our PrivatePCH in use
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				"Private",
				"Private/WeatherActor",
				"Private/Module_ShaderTools",
				"Private/ShaderDebugUtils"
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "RenderCore",
                "RHI"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Projects", //<-Needed for IPluginManager, so we can get the shader directory in our plugin // Might not be needed anymore 4.21+?
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
