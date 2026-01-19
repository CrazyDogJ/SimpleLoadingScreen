// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SLoadingScreenLayout.h"

class SLoadingTextWidget;
class USimpleLoadingScreenSettings;

/**
 * Default simple loading screen
 */
class SIMPLELOADINGSCREEN_API SSimpleLoadingScreen : public SLoadingScreenLayout
{
public:
	SLATE_BEGIN_ARGS(SSimpleLoadingScreen) {}

	SLATE_END_ARGS()

	/**
	 * Construct this widget
	 */
	void Construct(const FArguments& InArgs, const USimpleLoadingScreenSettings* InSettings);

	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;	

	void FadeOut();
private:
	bool bHasFade = false;
	const USimpleLoadingScreenSettings* Settings = nullptr;
	FCurveSequence FadeAnimationSequence;
	TSharedPtr<SOverlay> Root;
	TSharedPtr<SHorizontalBox> HorizontalBox;
	TSharedPtr<SBorder> Border;
	
	SHorizontalBox::FSlot* TextSlot = nullptr;
	SHorizontalBox::FSlot* LoadingImageSlot = nullptr;
};
