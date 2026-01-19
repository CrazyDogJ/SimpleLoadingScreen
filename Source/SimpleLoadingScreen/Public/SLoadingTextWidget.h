#pragma once

#include "Widgets/SCompoundWidget.h"

struct FSimpleLoadingScreenLoadingText;
class FDeferredCleanupSlateBrush;

/**
 * Background widget
 */
class SLoadingTextWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingTextWidget) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const FSimpleLoadingScreenLoadingText* InSettings);

	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
protected:
	// Current total delta time
	mutable float TotalDeltaTime = 0.0f;

private:
	// Placehold widget
	TSharedRef<SWidget> TextWidget = SNullWidget::NullWidget;
	const FSimpleLoadingScreenLoadingText* Settings = nullptr;
};
