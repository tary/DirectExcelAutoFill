using UnrealBuildTool;
using System.IO;

public class DirectExcelEditor : ModuleRules
{
    public DirectExcelEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        bEnforceIWYU = false;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
            }
        );

        PrivateIncludePaths.AddRange(
            new[]
            {
                "DirectExcelEditor/Private",
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new[]
            {
                "Kismet",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "DirectExcel", 
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "ApplicationCore",
                "Projects",
                "Slate",
                "SlateCore",
                "DeveloperSettings",
            }
        );
    }
}