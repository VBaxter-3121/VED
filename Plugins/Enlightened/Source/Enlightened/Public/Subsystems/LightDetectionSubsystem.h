// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Light.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/DirectionalLightComponent.h"

#include "EngineUtils.h"
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LightDetectionSubsystem.generated.h"

UCLASS()
class ENLIGHTENED_API ULightDetectionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	struct LightProperties
	{
		// Properties of all lights (except sky lights and directional lights)
		ALight* Light;
		ULocalLightComponent* Component;
		FVector Location;
		float AttenRad;
		float Brightness;
		// Properties specific to spot lights
		bool IsSpotLight;
		float InnerCone;
		float OuterCone;
		// Properties specific to rect lights
		bool IsRectLight;
		float Width;
		float Height;
		float BarnAngle;
		float BarnLength;
	};

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	const TArray<LightProperties>& GetLights() const { return PropertiesArray; }
	const float GetLargestAttenRad() const { return LargestAttenRad; }

private:
	TArray<LightProperties> PropertiesArray;
	float LargestAttenRad = 0;
};
