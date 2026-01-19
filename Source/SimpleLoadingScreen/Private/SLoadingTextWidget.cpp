#include "SLoadingTextWidget.h"
#include "SimpleLoadingScreenSettings.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Engine/Texture2D.h"

void SLoadingTextWidget::Construct(const FArguments& InArgs, const FSimpleLoadingScreenLoadingText* InSettings)
{
	if (!InSettings)
	{
		return;
	}
	
	Settings = InSettings;
	ChildSlot
	[
		TextWidget = SNew(STextBlock)
		.Font(InSettings->Font)
		.Text(InSettings->DefaultText)
		.ColorAndOpacity(InSettings->Color)
	];
}

int32 SLoadingTextWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (!Settings)
		return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	
	StaticCastSharedRef<STextBlock>(TextWidget)->SetColorAndOpacity(Settings->Color);
	StaticCastSharedRef<STextBlock>(TextWidget)->SetFont(Settings->Font);
	StaticCastSharedRef<STextBlock>(TextWidget)->SetText(Settings->DefaultText);
	
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}
