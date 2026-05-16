// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LightDetectionComponent.h"

// Sets default values for this component's properties
ULightDetectionComponent::ULightDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void ULightDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the current level
	World = GetWorld();
	// Validate level
	if (!World)	return;
	
	// Get a reference to the Light detection system
	ULightDetectionSubsystem* LightSystem = GetWorld()->GetSubsystem<ULightDetectionSubsystem>();
	// Validate light detection system
	if (!LightSystem) return;

	// Fetch data from light detection system
	Lights = LightSystem->GetLights();
	LargestAttenRad = LightSystem->GetLargestAttenRad();
}

// Called every frame
void ULightDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Run CheckIllumination based on Frequency for optimisation
	CurrentInterval += DeltaTime;
	if (CurrentInterval >= Frequency)
	{
		CheckIllumination();
		CurrentInterval = 0;
	}
}

void ULightDetectionComponent::CheckIllumination()
{
	UE_LOG(LogTemp, Warning, TEXT("Lights.Num() = %d"), Lights.Num());

	// Variable for hit data
	FHitResult Hit;

	// Get world position of this component
	FVector Start = GetComponentLocation();

	// Default parameters
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	// Default response parameters
	FCollisionResponseParams ResponseParams;

	Illuminated = false;

	// Iterate through all light sources in range
	for (int i = 0; i < Lights.Num(); ++i)
	{
		// Unpack struct
		ALight* Light = Lights[i].Light;
		ULocalLightComponent* Component = Lights[i].Component;
		FVector Location = Lights[i].Location;
		float AttenRad = Lights[i].AttenRad;
		float Brightness = Lights[i].Brightness;

		// Ignore lights further away than the largest attenuation radius in the level
		float LightDistance = FVector::Dist(GetComponentLocation(), Component->GetComponentLocation());

		if (LightDistance <= LargestAttenRad && Component->IsVisible())
		{
			// Debug code
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Found light: %s"), *Light->GetName()));

			// Check for line of sight
			bool BlockedLOS = World->LineTraceSingleByChannel(Hit, Start, Location, ECC_Visibility, Params, ResponseParams);

			// Debug code
			if (ShowDebugLines)
			{
				DrawDebugLine(World, Start, Location, FColor::Red, false, 2.0f, 0, 1.5f);
			}
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Line trace from %s to %s %s"), *GetReadableName(), *Light->GetName(), LOS ? TEXT("is blocked.") : TEXT("is not blocked.")));
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, !BlockedLOS ? TEXT("True") : TEXT("False"));

			// If component has line of sight to light source, check if illuminated
			if (!BlockedLOS)
			{
				// Calculate light falloff
				float Normalized = 1.0f - FMath::Square(LightDistance / AttenRad);
				Normalized = FMath::Clamp(Normalized, 0.0f, 1.0f);
				float Falloff = Normalized * Normalized;

				// Irradiance: "In radiometry, irradiance is the radiant flux received by a surface per unit area."
				float Irradiance = (Brightness / FMath::Max(LightDistance * LightDistance, 1.0f)) * Falloff;

				if (Lights[i].IsSpotLight)
				{
					float OuterCone = Lights[i].OuterCone;
					float InnerCone = Lights[i].InnerCone;

					// Vector from light to component
					FVector ToComponent = (GetComponentLocation() - Light->GetActorLocation()).GetSafeNormal();
					FVector SpotDir = Light->GetActorForwardVector().GetSafeNormal();

					// Cosine of angle between spotlight direction and target direction
					float CosAngle = FVector::DotProduct(SpotDir, ToComponent);

					// Convert inner/outer cone angles to cosines
					float InnerCos = FMath::Cos(FMath::DegreesToRadians(InnerCone * 0.5f));
					float OuterCos = FMath::Cos(FMath::DegreesToRadians(OuterCone * 0.5f));

					// Smooth Hermite interpolation (Unreal-style falloff)
					float AngularFalloff = 0.0f;

					if (CosAngle >= InnerCos)
					{
						AngularFalloff = 1.0f;
					}
					else if (CosAngle > OuterCos)
					{
						float T = (CosAngle - OuterCos) / (InnerCos - OuterCos);
						T = FMath::Clamp(T, 0.0f, 1.0f);

						// Unreal-style smoothstep
						AngularFalloff = T * T * (3.0f - 2.0f * T);
					}
					else
					{
						AngularFalloff = 0.0f;
					}

					Irradiance *= AngularFalloff;
				}
				else if (Lights[i].IsRectLight)
				{
					float Width = Lights[i].Width;
					float Height = Lights[i].Height;
					float BarnAngle = Lights[i].BarnAngle;
					float BarnLength = Lights[i].BarnLength;

					//DrawDebugLine(World, Lights[i].Location, Lights[i].Location + Lights[i].Light->GetActorForwardVector() * 1000, FColor::Red, false, 2.0f, 0, 1.5f);
				}

				// Determine where the boundary of irradiance lies
				if (Irradiance > IlluminationThreshold)
				{
					Illuminated = true;
					if (PrintIlluminated)
					{
						GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, FString::Printf(TEXT("%s: In light radius of %s"), *GetName(), *Light->GetName()));
					}
				}
			}
		}
	}
}