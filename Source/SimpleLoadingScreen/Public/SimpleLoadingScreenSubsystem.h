// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleLoadingScreenSettings.h"
#include "SSimpleLoadingScreen.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "SimpleLoadingScreenSubsystem.generated.h"

UCLASS()
class SIMPLELOADINGSCREEN_API USimpleLoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	TSharedPtr<SDPIScaler> Slot;
	TSharedPtr<SSimpleLoadingScreen> LoadingScreen;

	UPROPERTY()
	UUserWidget* CachedLoadingScreenUserWidget;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void PreSetupLoadingScreen();
	void MoviePlaybackFinished();
	void PostLoadMapWithWorld(UWorld* World);
	void SetupLoadingScreen(const FSimpleLoadingScreenAttributes& LoadingScreenSettings);

public:
	bool IsSlotValid() const { return Slot.IsValid(); }
	TSharedPtr<SDPIScaler> GetSlot() const { return Slot; }
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 DisplayBackgroundIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowLoadingScreen = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAutoHideLoadingScreen = true;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bLoadingScreenValid = false;

	UFUNCTION(BlueprintCallable, Category = "Simple Loading Screen")
	float GetFadeAnimationTime();
	
	UFUNCTION(BlueprintCallable, Category = "Simple Loading Screen")
	void ShowLoadingScreen(bool InAutoHideLoadingScreen = true);

	UFUNCTION(BlueprintCallable, Category = "Simple Loading Screen")
	void HideLoadingScreen();
	
	void HideLoadingScreenInternal();
};
