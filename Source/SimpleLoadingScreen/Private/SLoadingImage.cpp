// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadingImage.h"

#include "SimpleLoadingScreenSettings.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#if ENGINE_MAJOR_VERSION == 4
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Images/SImage.h"
#endif

void SLoadingImage::Construct(const FArguments& InArgs, const FLoadingImageSequenceType* InSettings)
{
	Settings = InSettings;
	if (Settings == nullptr)
	{
		return;
	}
	
	// Loading Widget is image sequence
	if (InSettings->SequenceTextures.Num() > 0)
	{
		CleanupBrushList.Empty();
		ImageIndex = 0;
		
		for (auto Image : InSettings->SequenceTextures)
		{
			const auto LoadedImage = Image.LoadSynchronous();
#if ENGINE_MAJOR_VERSION == 4
			CleanupBrushList.Add(FDeferredCleanupSlateBrush::CreateBrush(LoadedImage,
				FVector2D(Image->PlatformData->SizeX * InSettings->Scale, Image->PlatformData->SizeY * InSettings->Scale)
				));
#else
			CleanupBrushList.Add(FDeferredCleanupSlateBrush::CreateBrush(LoadedImage,
				FVector2f(Image->GetPlatformData()->SizeX * InSettings->Scale, Image->GetPlatformData()->SizeY * InSettings->Scale)
				));
#endif
		}
		
		// Create Image slate widget
		LoadingIcon = SNew(SImage)
			.Image(CleanupBrushList[ImageIndex]->GetSlateBrush());

		// Update play animation interval
		Interval = InSettings->Period;
	}
	else
	{
		// If there is no image in the array then create a spacer instead
		LoadingIcon = SNew(SSpacer).Size(FVector2D::ZeroVector);
	}
	
	ChildSlot
	[
		LoadingIcon
	];
}

int32 SLoadingImage::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	TotalDeltaTime += Args.GetDeltaTime();
	if (!Settings)
	{
		return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	}
	
	if (TotalDeltaTime >= Settings->Period)
	{
		if (CleanupBrushList.Num() > 1)
		{
			if (bPlayReverse)
			{
				ImageIndex--;
			}
			else
			{
				ImageIndex++;
			}

			if (ImageIndex >= CleanupBrushList.Num())
			{
				ImageIndex = 0;
			}
			else if (ImageIndex < 0)
			{
				ImageIndex = CleanupBrushList.Num() - 1;
			}

			const auto Brush = CleanupBrushList[ImageIndex].IsValid() ? CleanupBrushList[ImageIndex]->GetSlateBrush() : nullptr;
			StaticCastSharedRef<SImage>(LoadingIcon)->SetImage(Brush);
		}

		TotalDeltaTime = 0.0f;
	}
	

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void ULoadingImage::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyLoadingImage.Reset();
}

TSharedRef<SWidget> ULoadingImage::RebuildWidget()
{
	const auto Settings = GetDefault<USimpleLoadingScreenSettings>();
	const auto* LoadingImageSetting = Settings->LoadingImage.ImageSequences.IsValidIndex(0) ? &Settings->LoadingImage.ImageSequences[0] : nullptr;
	
	MyLoadingImage = SNew(SLoadingImage, LoadingImageSetting);
	return MyLoadingImage.ToSharedRef();
}
