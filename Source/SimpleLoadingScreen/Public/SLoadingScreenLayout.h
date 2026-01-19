#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Engine/GameViewportClient.h"

/**
 * SLoadingScreenLayout
 * 
 * This is the base class for all loading screen layouts.
 * It provides common functionality such as DPI scaling and point size conversion.
 * 
 * Note: This class is not intended to be instantiated directly.
 * Instead, use derived classes that implement specific loading screen layouts.
 *
 */
class SIMPLELOADINGSCREEN_API SLoadingScreenLayout : public SCompoundWidget
{
public:	
	static float PointSizeToSlateUnits(float PointSize);
	float GetDPIScale() const;
protected:
	void CalculateViewportSize() const;
private:
	mutable FIntPoint _cachedViewportSize;
};
