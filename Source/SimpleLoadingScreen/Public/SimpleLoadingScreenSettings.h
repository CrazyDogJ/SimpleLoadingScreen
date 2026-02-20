// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoviePlayer.h"
#include "Engine/DeveloperSettings.h"
#include "Widgets/Layout/SScaleBox.h"
#include "SimpleLoadingScreenSettings.generated.h"

USTRUCT(BlueprintType)
struct FSimpleLoadingScreenAttributes
{
	GENERATED_BODY()

	/** The movie paths local to the game's Content/Movies/ directory we will play. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Attributes")
	TArray<FString> MoviePaths = {};

	/** The minimum time that a loading screen should be opened for. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Attributes")
	float MinimumLoadingScreenDisplayTime = -1.0f;

	/** If true, the loading screen will disappear as soon as all movies are played and loading is done. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Attributes")
	bool bAutoCompleteWhenLoadingCompletes = true;

	/** If true, movies can be skipped by clicking the loading screen as long as loading is done. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Attributes")
	bool bMoviesAreSkippable = true;

	/** If true, movie playback continues until Stop is called. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Attributes")
	bool bWaitForManualStop = false;

	/** If true loading screens here cannot have any uobjects of any kind or use any engine features at all. This will start the movies very early as a result on platforms that support it */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Attributes")
	bool bAllowInEarlyStartup = false;

	/** If true, this will call the engine tick while the game thread is stalled waiting for a loading movie to finish. This only works for post-startup load screens and is potentially unsafe */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Attributes")
	bool bAllowEngineTick = false;

	/** Should we just play back, loop, etc.  NOTE: if the playback type is MT_LoopLast, then bAutoCompleteWhenLoadingCompletes will be togged on when the last movie is hit*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Attributes")
	TEnumAsByte<EMoviePlaybackType> PlaybackType = EMoviePlaybackType::MT_Normal;
};

USTRUCT(BlueprintType)
struct FSimpleLoadingScreenBackground
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Background", meta = (AllowedClasses = "/Script/Engine.Texture2D"))
	TArray<FSoftObjectPath> BackgroundTextures;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Config, EditAnywhere, Category = "General")
	int32 BackgroundPreviewIndex = 0;
#endif
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Background")
	TEnumAsByte<EStretch::Type> ImageStretch = EStretch::ScaleToFit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Background")
	FMargin Padding;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Background")
	FLinearColor BackgroundColor = FLinearColor::Black;
};

USTRUCT(BlueprintType)
struct FSimpleLoadingScreenLoadingText
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Text")
	FSlateColor Color;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Text")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Bottom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Text")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Right;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Text")
	FSlateFontInfo Font;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Text")
	FText DefaultText;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Text")
	FMargin Padding = 10.0f;
};

USTRUCT(BlueprintType)
struct FLoadingImageSequenceType
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Image Sequence")
	TArray<TSoftObjectPtr<UTexture2D>> SequenceTextures;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Image Sequence")
	float Period = 0.05f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Image")
	float Scale = 1.0f;
};

USTRUCT(BlueprintType)
struct FSimpleLoadingScreenLoadingImage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Image")
	TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Bottom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Image")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Right;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Image")
	TArray<FLoadingImageSequenceType> ImageSequences;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Loading Screen Loading Image")
	FMargin Padding = 10.0f;
};

/**
 * 
 */
UCLASS(Config = "SimpleLoadingScreen", defaultconfig, meta = (DisplayName = "Simple Loading Screen"))
class SIMPLELOADINGSCREEN_API USimpleLoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	virtual FName GetCategoryName() const override;

	UPROPERTY(Config, EditAnywhere, Category = "UMG")
	TSubclassOf<UUserWidget> LoadingScreenUserWidget;
	
	UPROPERTY(Config, EditAnywhere, Category = "General")
	FSimpleLoadingScreenAttributes LoadingScreenAttributes;

	UPROPERTY(Config, EditAnywhere, Category = "General")
	FSimpleLoadingScreenBackground BackgroundSettings;

	UPROPERTY(Config, EditAnywhere, Category = "General")
	FSimpleLoadingScreenLoadingText LoadingTextSettings;

	UPROPERTY(Config, EditAnywhere, Category = "General")
	FSimpleLoadingScreenLoadingImage LoadingImage;
	
	UPROPERTY(Config, EditAnywhere, Category = "General")
	float FadeInAnimationDuration = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category = "General")
	FLinearColor BorderColor = FLinearColor(0, 0, 0, 0.2);

};
