// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/LightDetectionSubsystem.h"

void ULightDetectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULightDetectionSubsystem::Deinitialize()
{
	PropertiesArray.Empty();
	Super::Deinitialize();
}

void ULightDetectionSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	UWorld* World = GetWorld();
	if (!World) return;

	for (TActorIterator<ALight> i(World); i; ++i)
	{
		ALight* Light = *i;
		ULocalLightComponent* Component = Cast<ULocalLightComponent>(Light->GetLightComponent());
		if (!Component)
			continue;
		
		bool IsSpotLight = false;
		float InnerCone = 0;
		float OuterCone = 0;
		bool IsRectLight = false;
		float Width = 0;
		float Height = 0;
		float BarnAngle = 0;
		float BarnLength = 0;

		USpotLightComponent* SpotLight = Cast<USpotLightComponent>(Component);
		URectLightComponent* RectLight = Cast<URectLightComponent>(Component);

		if (SpotLight)
		{
			IsSpotLight = true;
			InnerCone = SpotLight->InnerConeAngle;
			OuterCone = SpotLight->OuterConeAngle;
		}
		else if (RectLight)
		{
			IsRectLight = true;
			Width = RectLight->SourceWidth;
			Height = RectLight->SourceHeight;
			BarnAngle = RectLight->BarnDoorAngle;
			BarnLength = RectLight->BarnDoorLength;
		}

		PropertiesArray.Add({
			Light, Component, Light->GetActorLocation(), 
			Component->AttenuationRadius, Component->ComputeLightBrightness(), 
			IsSpotLight, InnerCone, OuterCone, 
			IsRectLight, Width, Height, BarnAngle, BarnLength
			});

		if (Component->AttenuationRadius > LargestAttenRad)
		{
			LargestAttenRad = Component->AttenuationRadius;
		}
	}
}