// Fill out your copyright notice in the Description page of Project Settings.


#include "SSimpleLoadingScreen.h"

#include "SBackgroundWidget.h"
#include "SimpleLoadingScreenSettings.h"
#include "SimpleLoadingScreenSubsystem.h"
#include "SLoadingTextWidget.h"
#include "Widgets/Images/SThrobber.h"

void SSimpleLoadingScreen::Construct(const FArguments& InArgs, const USimpleLoadingScreenSettings* InSettings)
{
	if (!InSettings)
	{
		return;
	}

	Settings = InSettings;
	const FSimpleLoadingScreenBackground* BGSettingsPtr = &InSettings->BackgroundSettings;
	const FSimpleLoadingScreenLoadingText* TextSettingsPtr = &InSettings->LoadingTextSettings;
	
	HorizontalBox =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(TextSettingsPtr->VerticalAlignment)
		.HAlign(TextSettingsPtr->HorizontalAlignment)
		.Padding(TextSettingsPtr->Padding)
		.Expose(TextSlot)
		.AutoWidth()
		[
			SNew(SLoadingTextWidget, TextSettingsPtr)
		]
		+ SHorizontalBox::Slot()
		.VAlign(InSettings->LoadingImage.VerticalAlignment)
		.HAlign(InSettings->LoadingImage.HorizontalAlignment)
		.AutoWidth()
		.Expose(LoadingImageSlot)
		.Padding(InSettings->LoadingImage.Padding)
		[
			SNew(SThrobber)
		];

	FSlateBrush Brush;
	Brush.TintColor = InSettings->BorderColor;
	
	Border =
		SNew(SBorder)
		.VAlign(TextSettingsPtr->VerticalAlignment)
		.HAlign(TextSettingsPtr->HorizontalAlignment)
		.BorderImage(&Brush)
		[
			HorizontalBox.ToSharedRef()
		];
		
	// Root widget and background
	Root = SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBackgroundWidget, BGSettingsPtr)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		[
			Border.ToSharedRef()
		];

	// Add root to this widget
	ChildSlot
	[
		Root.ToSharedRef()
	];

	if (InSettings->FadeInAnimationDuration > 0.0f && !USimpleLoadingScreenSubsystem::bLoadingScreenValid)
	{
		bHasFade = true;
		FadeAnimationSequence = FCurveSequence(0.f, InSettings->FadeInAnimationDuration);
		FadeAnimationSequence.Play(AsShared(), false, 0.f, false);
	}
}

int32 SSimpleLoadingScreen::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	Border->SetVAlign(Settings->LoadingTextSettings.VerticalAlignment);
	Border->SetHAlign(Settings->LoadingTextSettings.HorizontalAlignment);

#if ENGINE_MAJOR_VERSION == 4
	TextSlot->Padding(Settings->LoadingTextSettings.Padding);
	TextSlot->VAlign(Settings->LoadingTextSettings.VerticalAlignment);
	TextSlot->HAlign(Settings->LoadingTextSettings.HorizontalAlignment);

	LoadingImageSlot->Padding(Settings->LoadingImage.Padding);
	LoadingImageSlot->VAlign(Settings->LoadingImage.VerticalAlignment);
	LoadingImageSlot->HAlign(Settings->LoadingImage.HorizontalAlignment);
#else
	TextSlot->SetPadding(Settings->LoadingTextSettings.Padding);
	TextSlot->SetVerticalAlignment(Settings->LoadingTextSettings.VerticalAlignment);
	TextSlot->SetHorizontalAlignment(Settings->LoadingTextSettings.HorizontalAlignment);

	LoadingImageSlot->SetPadding(Settings->LoadingImage.Padding);
	LoadingImageSlot->SetVerticalAlignment(Settings->LoadingImage.VerticalAlignment);
	LoadingImageSlot->SetHorizontalAlignment(Settings->LoadingImage.HorizontalAlignment);
#endif
	
	FSlateBrush Brush;
	Brush.TintColor = Settings->BorderColor;
	Border->SetBorderImage(&Brush);
	
	if (bHasFade)
	{
		Root->SetRenderOpacity(FadeAnimationSequence.GetLerp());
	}
	
	return SLoadingScreenLayout::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                                     bParentEnabled);
}

void SSimpleLoadingScreen::FadeOut()
{
	if (!Settings)
	{
		return;
	}

	if (Settings->FadeInAnimationDuration > 0.0f)
	{
		FadeAnimationSequence.PlayReverse(AsShared(), false, 0.f, false);
	}
}
