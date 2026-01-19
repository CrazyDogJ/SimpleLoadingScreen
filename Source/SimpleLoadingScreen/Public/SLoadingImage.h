// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleLoadingScreenSettings.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "UObject/Object.h"

/**
 * Loading image slate.
 */
class SLoadingImage : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingImage) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const FLoadingImageSequenceType* InSettings);

	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

protected:
	const FLoadingImageSequenceType* Settings = nullptr;
	// Placeholder widgets
	TSharedRef<SWidget> LoadingIcon = SNullWidget::NullWidget;
	// Image slate brush list
	TArray<TSharedPtr<FDeferredCleanupSlateBrush>> CleanupBrushList;

	// Play image sequence in reverse
	bool bPlayReverse = false;

	// Current image sequence index
	mutable int32 ImageIndex = 0;

	// Current total delta time
	mutable float TotalDeltaTime = 0.0f;

	//Time in second to update the images, the smaller value the faster of the animation. A zero value will update the images every frame.
	float Interval = 0.05f;	
};
