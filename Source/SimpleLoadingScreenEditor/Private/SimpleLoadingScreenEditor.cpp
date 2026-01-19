#include "SimpleLoadingScreenEditor.h"

#include "SSimpleLoadingScreen.h"
#include "SimpleLoadingScreenSettings.h"
#if ENGINE_MAJOR_VERSION == 4
#include "ToolMenus.h"
#endif
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "FSimpleLoadingScreenEditorModule"

const FName FSimpleLoadingScreenEditorModule::LoadingScreenTabName(TEXT("SimpleLoadingScreen"));

void FSimpleLoadingScreenEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(LoadingScreenTabName, 
		FOnSpawnTab::CreateRaw(this, &FSimpleLoadingScreenEditorModule::OnSpawnLoadingScreenTab))
		.SetDisplayName(NSLOCTEXT("SimpleLoadingScreen", "TabTitle", "Simple Loading Screen Preview"))
		.SetTooltipText(NSLOCTEXT("SimpleLoadingScreen", "TabTooltip", "Open Simple Loading Screen Preview Window"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory());
	
	RegisterMenuExtensions();
}

void FSimpleLoadingScreenEditorModule::ShutdownModule()
{
	UnregisterMenuExtensions();
	
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(LoadingScreenTabName);
}

void FSimpleLoadingScreenEditorModule::RegisterMenuExtensions()
{
	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		return;
	}

	if (UToolMenu* WindowMenu = ToolMenus->FindMenu("LevelEditor.MainMenu.Window"))
	{
		FToolMenuSection& Section = WindowMenu->FindOrAddSection("WindowLayout");
		Section.AddMenuEntry(
			"OpenSimpleLoadingScreenPreviewWindow",
			NSLOCTEXT("SimpleLoadingScreen", "MenuEntryTitle", "Simple Loading Screen"),
			NSLOCTEXT("SimpleLoadingScreen", "MenuEntryTooltip", "Open Simple Loading Screen Preview Window"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FSimpleLoadingScreenEditorModule::OpenLoadingScreenWindow))
		);
	}
}

void FSimpleLoadingScreenEditorModule::UnregisterMenuExtensions()
{
	if (UToolMenus* ToolMenus = UToolMenus::Get())
	{
		ToolMenus->RemoveSection("LevelEditor.MainMenu.Window", "OpenSimpleLoadingScreenPreviewWindow");
	}
}

void FSimpleLoadingScreenEditorModule::OpenLoadingScreenWindow()
{
	FGlobalTabmanager::Get()->TryInvokeTab(LoadingScreenTabName);
}

TSharedRef<SDockTab> FSimpleLoadingScreenEditorModule::OnSpawnLoadingScreenTab(const FSpawnTabArgs& Args)
{
	const auto Settings = GetDefault<USimpleLoadingScreenSettings>();
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SSimpleLoadingScreen, Settings)
		];
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSimpleLoadingScreenEditorModule, SimpleLoadingScreenEditor)