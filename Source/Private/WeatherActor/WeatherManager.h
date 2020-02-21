#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"

#include "GlobalShader.h"


#include "WeatherManager.generated.h"


struct FGenerateMipsStruct2;

/** Parameters for generating mip maps */
struct FGenerateMipsParams2
{
	ESamplerFilter Filter = SF_Bilinear;
	ESamplerAddressMode AddressU = AM_Clamp;
	ESamplerAddressMode AddressV = AM_Clamp;
	ESamplerAddressMode AddressW = AM_Clamp;
};

//An actor based weather system for simulating weather
UCLASS(Blueprintable, BlueprintType)
class AWeatherManager : public AWeatherManager_Properties
{
	GENERATED_UCLASS_BODY()

public:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Setup();

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Calculate(
			/* input */UPARAM(ref) TArray<FStruct_AirGridContainer_CPU>& input,
			/* output */TArray<FStruct_AirGridContainer_CPU>& output);

//Public function for executing the generate mips compute shader 
//Default sampler is always bilinear clamp
	void Execute(FRHICommandListImmediate& RHICmdList, FRHITexture* InTexture, const FGenerateMipsParams2& InParams = FGenerateMipsParams2());

	void Execute(class FRDGBuilder* GraphBuilder, class FRDGTexture* InGraphTexture, FRHISamplerState* InSampler);


protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



private:

	void Compute(FRHICommandListImmediate& RHIImmCmdList, FRHITexture* InTexture);

	FGenerateMipsStruct2* SetupTexture(FRHITexture* InTexture, const FGenerateMipsParams2& InParams = FGenerateMipsParams2());

};