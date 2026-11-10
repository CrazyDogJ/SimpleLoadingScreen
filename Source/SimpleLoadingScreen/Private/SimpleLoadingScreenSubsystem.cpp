// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleLoadingScreenSubsystem.h"

#include "SimpleLoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Layout/SDPIScaler.h"

void USimpleLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	bShowLoadingScreen = true;

	// 修复：专用服务器和无 Slate 环境不能构造加载控件。
	if (IsRunningDedicatedServer() || !FSlateApplication::IsInitialized()) return;
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
	// 修复：先撤销全局事件、定时器和窗口覆盖层，再让 GC 管理 UMG 控件。
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	if (!IsRunningDedicatedServer() && IsMoviePlayerEnabled() && GetMoviePlayer())
	{
		GetMoviePlayer()->OnPrepareLoadingScreen().RemoveAll(this);
		GetMoviePlayer()->OnMoviePlaybackFinished().RemoveAll(this);
	}
	HideLoadingScreenInternal();
	LoadingScreenHolder.Reset();
	LoadingScreen.Reset();
	UserWidget = nullptr;
	Super::Deinitialize();
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
	if (Settings->LoadingScreenUserWidget)
	{
		UserWidget = CreateWidget(GetGameInstance(), Settings->LoadingScreenUserWidget);
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
	// 修复：只响应本游戏实例加载完成的世界。
	if (World && World->GetGameInstance() == GetGameInstance()) MoviePlaybackFinished();
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
	
	// 修复：自定义 UMG 路径没有默认 Slate 动画，窗口建立前也不能登记覆盖层。
	if (!LoadingScreenHolder.IsValid() || !GetGameInstance()) return;
	auto& Timers = GetGameInstance()->GetTimerManager();
	const bool bWasHiding = Timers.IsTimerActive(HideTimer);
	Timers.ClearTimer(HideTimer);
	if (!bLoadingScreenValid)
	{
		const auto Viewport = GetGameInstance()->GetGameViewportClient();
		Window = Viewport ? Viewport->GetWindow() : nullptr;
		if (!Window.IsValid()) return;
		Window->AddOverlaySlot()[LoadingScreenHolder.ToSharedRef()];
		bLoadingScreenValid = true;
	}
	else if (!bWasHiding)
	{
		return;
	}
	if (LoadingScreen.IsValid()) LoadingScreen->FadeIn();
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
				GetGameInstance()->GetTimerManager().SetTimer(HideTimer, this, &ThisClass::HideLoadingScreenInternal,
					Settings->FadeInAnimationDuration, false);
			}
			else
			{
				HideLoadingScreenInternal();
			}
		}
	}
}

void USimpleLoadingScreenSubsystem::RemoveLoadingScreen()
{
	HideLoadingScreenInternal();
}

void USimpleLoadingScreenSubsystem::HideLoadingScreenInternal()
{
	if (GetGameInstance()) GetGameInstance()->GetTimerManager().ClearTimer(HideTimer);
	// Final stand for removing loading screen.
	if (LoadingScreenHolder.IsValid())
	{
		if (Window)
		{
			Window->RemoveOverlaySlot(LoadingScreenHolder.ToSharedRef());
		}
	}
	
	Window.Reset();
	bLoadingScreenValid = false;
}
