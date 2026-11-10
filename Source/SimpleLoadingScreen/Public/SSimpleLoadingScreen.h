// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SLoadingScreenLayout.h"

class USimpleLoadingScreenSubsystem;
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
	void Construct(const FArguments& InArgs, const USimpleLoadingScreenSettings* InSettings, const USimpleLoadingScreenSubsystem* InLoadingScreenSubsystem);

	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;	

	void FadeIn();
	void FadeOut();
private:
	// 修复：SBorder 保存画刷指针，画刷必须与控件同寿命。
	mutable FSlateBrush BorderBrush;
	bool bHasFade = false;
	const USimpleLoadingScreenSettings* Settings = nullptr;
	FCurveSequence FadeAnimationSequence;
	TSharedPtr<SOverlay> Root;
	TSharedPtr<SCompoundWidget> LoadingImageWidget;
	TSharedPtr<SHorizontalBox> HorizontalBox;
	TSharedPtr<SBorder> Border;

	
	SHorizontalBox::FSlot* TextSlot = nullptr;
	SHorizontalBox::FSlot* LoadingImageSlot = nullptr;
};
