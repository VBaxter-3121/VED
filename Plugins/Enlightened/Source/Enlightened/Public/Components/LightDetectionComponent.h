// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineUtils.h"
#include "Engine/Light.h"
#include "Engine/World.h"

#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Engine/RectLight.h"
#include "Engine/DirectionalLight.h"

#include "Subsystems/LightDetectionSubsystem.h"

#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/DirectionalLightComponent.h"

#include "Kismet/KismetSystemLibrary.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LightDetectionComponent.generated.h"

UCLASS( ClassGroup=(Lighting), meta=(BlueprintSpawnableComponent), DefaultToInstanced, EditInlineNew )
class ENLIGHTENED_API ULightDetectionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULightDetectionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Detection: Settings", meta = (ToolTip = "Minimum light intensity (as a percentage) required to be considered illuminated."))
	float IlluminationThreshold = 0.02;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Detection: Settings", meta = (ToolTip = "Frequency of light detection checks in seconds."))
	float Frequency = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Detection: Debugging", meta = (ToolTip = "Draws debug lines from this component to light sources in range."))
	bool ShowDebugLines = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Detection: Debugging", meta = (ToolTip = "If this component is illuminated, prints the name of this component and the light source illuminating it."))
	bool PrintIlluminated = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowProtectedAcces="true"))
	bool Illuminated;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	UWorld* World;
	TArray<ULightDetectionSubsystem::LightProperties> Lights;
	float LargestAttenRad;

	float CurrentInterval = 0;
	void CheckIllumination();
};

enum class ELightType : uint8
{
	Directional UMETA(DisplayName = "Directional"),
	Point		UMETA(DisplayName = "Point"),
	Spot		UMETA(DisplayName = "Spot"),
	Rect		UMETA(DisplayName = "Rect")
};