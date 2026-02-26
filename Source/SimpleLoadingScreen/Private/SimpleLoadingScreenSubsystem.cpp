// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLoadingScreenSubsystem.h"

#include "SimpleLoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Layout/SDPIScaler.h"

void USimpleLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	bShowLoadingScreen = true;

	CreateUserWidget();
	MakeScriptSlate();
	MakeLoadingScreenHolder();
	
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

	// Try clearing.
	LoadingScreenHolder.Reset();
	UserWidget->ReleaseSlateResources(true);
	UserWidget->ConditionalBeginDestroy();
	UserWidget = nullptr;
	
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

void USimpleLoadingScreenSubsystem::MakeLoadingScreenHolder()
{
	LoadingScreenHolder = SNew(SDPIScaler).DPIScale(1.0);
	
	if (UserWidget)
	{
		LoadingScreenHolder->SetContent(UserWidget->TakeWidget());
	}
	else if (LoadingScreen)
	{
		LoadingScreenHolder->SetContent(LoadingScreen.ToSharedRef());
	}
}

void USimpleLoadingScreenSubsystem::MakeScriptSlate()
{
	const USimpleLoadingScreenSettings* Settings = GetDefault<USimpleLoadingScreenSettings>();
	if (!UserWidget)
	{
		LoadingScreen = SNew(SSimpleLoadingScreen, Settings, this);
	}
}

void USimpleLoadingScreenSubsystem::CreateUserWidget()
{
	const USimpleLoadingScreenSettings* Settings = GetDefault<USimpleLoadingScreenSettings>();
	UserWidget = CreateWidget(GetGameInstance(), Settings->LoadingScreenUserWidget);
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
	// Set up loading screen widget.
	if (UserWidget)
	{
		LoadingScreenAttributes.WidgetLoadingScreen = UserWidget->TakeWidget();
	}
	else if (LoadingScreen)
	{
		LoadingScreenAttributes.WidgetLoadingScreen = LoadingScreen;
	}
	else
	{
		LoadingScreenAttributes.WidgetLoadingScreen = SNew(SSimpleLoadingScreen, Settings, this);
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
	
	if (!bLoadingScreenValid)
	{
		GetGameInstance()->GetGameViewportClient()->GetWindow()->AddOverlaySlot()[LoadingScreenHolder.ToSharedRef()];
		
		bLoadingScreenValid = true;
	}
}

void USimpleLoadingScreenSubsystem::HideLoadingScreen()
{
	if (bLoadingScreenValid)
	{
		// Do not use custom logic if UserWidgetSlate is valid.
		if (UserWidget)
		{
			HideLoadingScreenInternal();
		}
		else
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
				HideLoadingScreenInternal();
			}
		}
	}
}

void USimpleLoadingScreenSubsystem::HideLoadingScreenInternal()
{
	// Final stand for removing loading screen.
	if (LoadingScreenHolder.IsValid())
	{
		GetGameInstance()->GetGameViewportClient()->GetWindow()->RemoveOverlaySlot(LoadingScreenHolder.ToSharedRef());
	}
	
	bLoadingScreenValid = false;
}
