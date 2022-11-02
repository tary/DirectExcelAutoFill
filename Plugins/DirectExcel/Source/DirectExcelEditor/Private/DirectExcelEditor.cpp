#include "DirectExcelEditorCommands.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "ToolMenus.h"
#include "Tool/ExcelAutoFillTool.h"

#define LOCTEXT_NAMESPACE "FDirectExcelEditorModule"


class FDirectExcelEditorModule : public IModuleInterface
{
public:

    virtual void StartupModule() override
    {
        FDirectExcelEditorCommands::Register();
        
        AutoFillTool = MakeShareable(new FExcelAutoFillTool());
        AutoFillTool->OnRegister();

        const TSharedPtr<FUICommandList> PluginCommands = MakeShareable(new FUICommandList);

        PluginCommands->MapAction(
            FDirectExcelEditorCommands::Get().RefreshTable,
            FExecuteAction::CreateStatic(&FDirectExcelEditorModule::RefreshTable_Execute),
            FCanExecuteAction());

        FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
        MenuExtender->AddMenuExtension("Tools", EExtensionHook::First, PluginCommands,
            FMenuExtensionDelegate::CreateRaw(this, &FDirectExcelEditorModule::AddMenuExtension));
        
        LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
        
        auto& MenuExtenders = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();

        MenuExtenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateStatic(&FDirectExcelEditorModule::OnExtendLevelEditorActorContextMenu));
        LevelViewportExtenderHandle = MenuExtenders.Last().GetHandle();
    }

    virtual void ShutdownModule() override
    {
        FDirectExcelEditorCommands::Unregister();

        if (LevelViewportExtenderHandle.IsValid())
        {
            if (FLevelEditorModule* LevelEditorModule = FModuleManager::Get().GetModulePtr<FLevelEditorModule>("LevelEditor"))
            {
                typedef FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors DelegateType;
                LevelEditorModule->GetAllLevelViewportContextMenuExtenders().RemoveAll([=](const DelegateType& In) { return In.GetHandle() == LevelViewportExtenderHandle; });
            }
            LevelViewportExtenderHandle.Reset();
        }
        if (AutoFillTool.IsValid())
        {
            AutoFillTool->OnUnRegister();
            AutoFillTool = nullptr;
        }
    }

    static void RefreshTable_Execute()
    {
        if (AutoFillTool.IsValid())
        {
            AutoFillTool->LoadTables();
        }
    }

    static void RefreshObject_Execute()
    {
        if (AutoFillTool.IsValid())
        {
            USelection* Selection = GEditor->GetSelectedActors();
            TArray<AActor*> AllSelectedActors;
            Selection->GetSelectedObjects(AllSelectedActors);
            for (AActor* SelectedActor : AllSelectedActors)
            {
                if (!SelectedActor)
                {
                    continue;
                }
                AutoFillTool->RefreshOnObject(SelectedActor, NAME_None);
            }
        }
    }

    static TSharedRef<FExtender> OnExtendLevelEditorActorContextMenu(const TSharedRef<FUICommandList> CommandList, const TArray<AActor*> SelectedActors)
    {
        TSharedRef<FExtender> Extender(new FExtender());
			   
		TArray<AActor*> SupportedActors;
		if (SelectedActors.Num() > 0)
		{
		    for (AActor* Actor : SelectedActors)
		    {
		        if (Actor && FExcelAutoFillTool::IsSupportedActor(Actor))
		        {
		            SupportedActors.Add(Actor);
		        }
		    }
		}

		if (SupportedActors.Num() > 0)
		{
			// Add asset actions extender
			Extender->AddMenuExtension("ActorOptions", EExtensionHook::After, CommandList,
			    FMenuExtensionDelegate::CreateStatic(&FDirectExcelEditorModule::CreateActorActionsMenu));
		}

		return Extender;
    }

    static void CreateActorActionsMenu(FMenuBuilder& Builder)
    {
        Builder.AddMenuEntry(FDirectExcelEditorCommands::Get().RefreshObject->GetLabel(),
                             FDirectExcelEditorCommands::Get().RefreshObject->GetDescription(),
                             FSlateIcon(),
                             FUIAction(FExecuteAction::CreateStatic(&FDirectExcelEditorModule::RefreshObject_Execute)));
    }

    void AddMenuExtension(FMenuBuilder& Builder)
    {
        Builder.AddMenuEntry(FDirectExcelEditorCommands::Get().RefreshTable);
    }
private:
    static TSharedPtr<FExcelAutoFillTool> AutoFillTool;
    FDelegateHandle LevelViewportExtenderHandle;
};

TSharedPtr<FExcelAutoFillTool> FDirectExcelEditorModule::AutoFillTool;

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDirectExcelEditorModule, DirectExcelEditor)
