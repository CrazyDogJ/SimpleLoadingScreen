#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSimpleLoadingScreenEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterMenuExtensions();
    void UnregisterMenuExtensions();
    
    void OpenLoadingScreenWindow();
    TSharedRef<SDockTab> OnSpawnLoadingScreenTab(const FSpawnTabArgs& Args);

private:
    TSharedPtr<FExtender> MenuExtender;
    static const FName LoadingScreenTabName;
};
