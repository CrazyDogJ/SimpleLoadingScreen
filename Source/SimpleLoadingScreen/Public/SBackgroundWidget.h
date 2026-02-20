#pragma once

#include "Widgets/SCompoundWidget.h"

class USimpleLoadingScreenSubsystem;
struct FSimpleLoadingScreenBackground;
class FDeferredCleanupSlateBrush;

/**
 * Background widget
 */
class SBackgroundWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBackgroundWidget) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const FSimpleLoadingScreenBackground* InSettings, const USimpleLoadingScreenSubsystem* InLoadingScreenSubsystem);

	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
protected:
	// Current total delta time
	mutable float TotalDeltaTime = 0.0f;

private:
	// Placehold widget
	TSharedRef<SWidget> BackgroundWidget = SNullWidget::NullWidget;
	TSharedRef<SWidget> ScalarBox = SNullWidget::NullWidget;
	TSharedRef<SWidget> Border = SNullWidget::NullWidget;

	const USimpleLoadingScreenSubsystem* LoadingScreenSubsystem = nullptr;
	
	const FSimpleLoadingScreenBackground* Settings = nullptr;
	
	mutable TSharedPtr<FDeferredCleanupSlateBrush> ImageBrush;
};
