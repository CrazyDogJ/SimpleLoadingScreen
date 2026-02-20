#include "SLoadingScreenLayout.h"
#include "Engine/UserInterfaceSettings.h"
#include "Engine/Engine.h"
#include "GenericPlatform/GenericApplication.h"

float SLoadingScreenLayout::PointSizeToSlateUnits(float PointSize)
{
	constexpr float SlateFreeTypeHorizontalResolutionDPI = 96.0f;
	constexpr float FreeTypeNativeDPI = 72.0;
	const float PixelSize = PointSize * (SlateFreeTypeHorizontalResolutionDPI / FreeTypeNativeDPI);
	return PixelSize;
}

float SLoadingScreenLayout::GetDPIScale() const
{
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		int32 X = FGenericPlatformMath::FloorToInt(ViewportSize.X);
		int32 Y = FGenericPlatformMath::FloorToInt(ViewportSize.Y);		

		if (X != 0 && Y != 0)
		{
			_cachedViewportSize = FIntPoint(X, Y);
		}
		else
		{
			CalculateViewportSize();
		}
	}
	else 
	{
		CalculateViewportSize();
	}
	
	return FMath::Clamp(GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(_cachedViewportSize), 0.1f, 1.0f);
}

void SLoadingScreenLayout::CalculateViewportSize() const
{
	const FVector2D DrawSize = GetTickSpaceGeometry().ToPaintGeometry().GetLocalSize();
	if (!DrawSize.Equals(FVector2D::ZeroVector))
	{
		const int32 X = FGenericPlatformMath::FloorToInt(DrawSize.X);
		const int32 Y = FGenericPlatformMath::FloorToInt(DrawSize.Y);
		_cachedViewportSize = FIntPoint(X, Y);
	}
}

