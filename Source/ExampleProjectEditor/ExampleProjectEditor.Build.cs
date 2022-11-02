using UnrealBuildTool;

public class ExampleProjectEditor : ModuleRules
{
    public ExampleProjectEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;


        PrivateDependencyModuleNames.AddRange(new string[] {
            "HTTP",
            "Json",
            "JsonUtilities",
            "EditorScriptingUtilities",
            "ContentBrowser",
            "Core",
            "ToolMenus",
            "ApplicationCore",
            "ContentBrowser",
            "Persona",
            "MaterialEditor", 
            "DirectExcel",
            "DirectExcelEditor",
        });
    }
}
