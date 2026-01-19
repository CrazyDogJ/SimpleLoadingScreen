#include "SBackgroundWidget.h"
#include "SimpleLoadingScreenSettings.h"
#include "SimpleLoadingScreenSubsystem.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Engine/Texture2D.h"

void SBackgroundWidget::Construct(const FArguments& InArgs, const FSimpleLoadingScreenBackground* InSettings)
{
	if (!InSettings)
	{
		return;
	}
	
	Settings = InSettings;
	
	// Setup default transparent texture;
	if (IsMoviePlayerEnabled())
	{
		
	}
	const auto Images = Settings->BackgroundTextures;
	const auto Index = IsMoviePlayerEnabled() ? USimpleLoadingScreenSubsystem::DisplayBackgroundIndex : Settings->BackgroundPreviewIndex;
	if (Images.IsValidIndex(Index))
	{
		// Load background from settings
		if (UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *Images[Index].ToString()))
		{
			ImageBrush = FDeferredCleanupSlateBrush::CreateBrush(Texture);
		}
	}
	
	ChildSlot
	[
		Border = SNew(SBorder)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(Settings->Padding)
		.BorderBackgroundColor(Settings->BackgroundColor)
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		[
			ScalarBox = SNew(SScaleBox)
			.Stretch(Settings->ImageStretch)
			[
				BackgroundWidget = SNew(SImage)
				.Image(ImageBrush.IsValid() ? ImageBrush->GetSlateBrush() : nullptr)
			]
		]
	];
}

int32 SBackgroundWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (!Settings)
		return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	StaticCastSharedRef<SScaleBox>(ScalarBox)->SetStretch(Settings->ImageStretch);
	StaticCastSharedRef<SBorder>(Border)->SetPadding(Settings->Padding);
	StaticCastSharedRef<SBorder>(Border)->SetBorderBackgroundColor(Settings->BackgroundColor);
	
	const auto Images = Settings->BackgroundTextures;
	// Load background from settings
	const auto Index = IsMoviePlayerEnabled() ? USimpleLoadingScreenSubsystem::DisplayBackgroundIndex : Settings->BackgroundPreviewIndex;
	if (Images.IsValidIndex(Index))
	{
		// Load background from settings
		if (UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *Images[Index].ToString()))
		{
			ImageBrush = FDeferredCleanupSlateBrush::CreateBrush(Texture);
			StaticCastSharedRef<SImage>(BackgroundWidget)->SetImage(ImageBrush.IsValid() ? ImageBrush->GetSlateBrush() : nullptr);
		}
	}
	
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}
