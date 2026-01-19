// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLoadingScreenSubsystem.h"

#include "SimpleLoadingScreenSettings.h"
#include "Widgets/Layout/SDPIScaler.h"

int32 USimpleLoadingScreenSubsystem::DisplayBackgroundIndex = 0;
bool  USimpleLoadingScreenSubsystem::bShowLoadingScreen = false;
bool  USimpleLoadingScreenSubsystem::bLoadingScreenValid = false;

void USimpleLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bShowLoadingScreen = true;
	
	if (!IsRunningDedicatedServer() && FSlateApplication::IsInitialized())
	{
		if (IsMoviePlayerEnabled())
		{
			GetMoviePlayer()->OnPrepareLoadingScreen().AddUObject(this, &USimpleLoadingScreenSubsystem::PreSetupLoadingScreen);
			GetMoviePlayer()->OnMoviePlaybackFinished().AddUObject(this, &USimpleLoadingScreenSubsystem::HideLoadingScreen);
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

void USimpleLoadingScreenSubsystem::ShowLoadingScreen()
{
	if (GEngine && GEngine->GameViewport && !bLoadingScreenValid)
	{
		const auto Settings = GetDefault<USimpleLoadingScreenSettings>();
		LoadingScreen = SNew(SSimpleLoadingScreen, Settings);
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

		bLoadingScreenValid = true;
	}
}

void USimpleLoadingScreenSubsystem::HideLoadingScreen()
{
	if (GEngine && GEngine->GameViewport && bLoadingScreenValid)
	{
		const auto Settings = GetDefault<USimpleLoadingScreenSettings>();
		if (Settings->FadeInAnimationDuration > 0.0f)
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
