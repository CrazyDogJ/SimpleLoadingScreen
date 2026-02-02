// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLoadingScreenSubsystem.h"

#include "SimpleLoadingScreenSettings.h"
#include "Widgets/Layout/SDPIScaler.h"

int32 USimpleLoadingScreenSubsystem::DisplayBackgroundIndex = 0;
bool  USimpleLoadingScreenSubsystem::bShowLoadingScreen = false;
bool  USimpleLoadingScreenSubsystem::bAutoHideLoadingScreen = true;
bool  USimpleLoadingScreenSubsystem::bLoadingScreenValid = false;

void USimpleLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bShowLoadingScreen = true;
	
	if (!IsRunningDedicatedServer() && FSlateApplication::IsInitialized())
	{
		if (IsMoviePlayerEnabled())
		{
			// First setup loading screen. Fixing pre-loading screen problem
			PreSetupLoadingScreen();
			GetMoviePlayer()->OnPrepareLoadingScreen().AddUObject(this, &USimpleLoadingScreenSubsystem::PreSetupLoadingScreen);
			GetMoviePlayer()->OnMoviePlaybackFinished().AddUObject(this, &USimpleLoadingScreenSubsystem::MoviePlaybackFinished);
		}
		else
		{
			// If in pie, we usually use post load map to auto hide the loading screen.
			FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USimpleLoadingScreenSubsystem::PostLoadMapWithWorld);
		}
	}
}

void USimpleLoadingScreenSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (!IsRunningDedicatedServer())
	{
		if (IsMoviePlayerEnabled())
		{
			GetMoviePlayer()->OnPrepareLoadingScreen().RemoveAll(this);
			GetMoviePlayer()->OnMoviePlaybackFinished().RemoveAll(this);
		}
		else
		{
			// If in pie, we usually use post load map to auto hide the loading screen.
			FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
		}
	}
}

void USimpleLoadingScreenSubsystem::PreSetupLoadingScreen()
{
	if (bShowLoadingScreen)
	{
		const USimpleLoadingScreenSettings* Settings = GetDefault<USimpleLoadingScreenSettings>();
		SetupLoadingScreen(Settings->LoadingScreenAttributes);
	}
}

void USimpleLoadingScreenSubsystem::MoviePlaybackFinished()
{
	if (bAutoHideLoadingScreen)
	{
		HideLoadingScreen();
	}
}

void USimpleLoadingScreenSubsystem::PostLoadMapWithWorld(UWorld* World)
{
	MoviePlaybackFinished();
}

void USimpleLoadingScreenSubsystem::SetupLoadingScreen(const FSimpleLoadingScreenAttributes& LoadingScreenSettings)
{
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = LoadingScreenSettings.MinimumLoadingScreenDisplayTime;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = LoadingScreenSettings.bAutoCompleteWhenLoadingCompletes;
	LoadingScreenAttributes.bMoviesAreSkippable = LoadingScreenSettings.bMoviesAreSkippable;
	LoadingScreenAttributes.bWaitForManualStop = LoadingScreenSettings.bWaitForManualStop;
	LoadingScreenAttributes.bAllowInEarlyStartup = LoadingScreenSettings.bAllowInEarlyStartup;
	LoadingScreenAttributes.bAllowEngineTick = LoadingScreenSettings.bAllowEngineTick;
	LoadingScreenAttributes.MoviePaths = LoadingScreenSettings.MoviePaths;
	LoadingScreenAttributes.PlaybackType = LoadingScreenSettings.PlaybackType;
	const auto Settings = GetDefault<USimpleLoadingScreenSettings>();
	if (LoadingScreen)
	{
		LoadingScreenAttributes.WidgetLoadingScreen = LoadingScreen;
	}
	else
	{
		LoadingScreenAttributes.WidgetLoadingScreen = SNew(SSimpleLoadingScreen, Settings);
	}

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

float USimpleLoadingScreenSubsystem::GetFadeAnimationTime()
{
	const auto Settings = GetDefault<USimpleLoadingScreenSettings>();
	return Settings->FadeInAnimationDuration;
}

void USimpleLoadingScreenSubsystem::ShowLoadingScreen(const bool InAutoHideLoadingScreen)
{
	bAutoHideLoadingScreen = InAutoHideLoadingScreen;
	
	if (GEngine && GEngine->GameViewport && !bLoadingScreenValid)
	{
		const auto Settings = GetDefault<USimpleLoadingScreenSettings>();
		LoadingScreen = SNew(SSimpleLoadingScreen, Settings);
		// Setup pre-loading screen will override the pre-loading screen effect. Fixing pre-loading screen problem
		PreSetupLoadingScreen();
		Slot =
			SNew(SDPIScaler).DPIScale(1.0)
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush(TEXT("NoBorder")))
				.Padding(0)
				[
					LoadingScreen.ToSharedRef()
				]
			];
		
		GEngine->GameViewport->GetWindow()->AddOverlaySlot()
		[
			Slot.ToSharedRef()
		];
		
#if WITH_EDITOR
		if (GetWorld()->IsPlayInEditor())
		{
			GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Yellow, FString::Printf(TEXT("Loading Screen Shown")));
		}
#endif
		
		bLoadingScreenValid = true;
	}
}

void USimpleLoadingScreenSubsystem::HideLoadingScreen()
{
	if (GEngine && GEngine->GameViewport && bLoadingScreenValid)
	{
		const auto Settings = GetDefault<USimpleLoadingScreenSettings>();
		if (Settings->FadeInAnimationDuration > 0.0f && LoadingScreen)
		{
			LoadingScreen->FadeOut();
			FTimerHandle DelayHandle;
			GetWorld()->GetTimerManager().SetTimer(DelayHandle, this, &ThisClass::HideLoadingScreenInternal,
				Settings->FadeInAnimationDuration, false);
		}
		else
		{
#if WITH_EDITOR
			if (GetWorld()->IsPlayInEditor())
			{
				GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Yellow, FString::Printf(TEXT("Hiding Screen Shown")));
			}
#endif
			HideLoadingScreenInternal();
		}
	}
}

void USimpleLoadingScreenSubsystem::HideLoadingScreenInternal()
{
	if (Slot.IsValid())
	{
		GEngine->GameViewport->GetWindow()->RemoveOverlaySlot(Slot.ToSharedRef());
	}
	
	Slot = nullptr;
	LoadingScreen = nullptr;
	bLoadingScreenValid = false;
}

void USimpleLoadingScreenSubsystem::SetAutoShowLoadingScreen(bool bShow)
{
	bShowLoadingScreen = bShow;
}

void USimpleLoadingScreenSubsystem::SetLoadingScreenBackgroundIndex(int Index)
{
	DisplayBackgroundIndex = Index;
}
