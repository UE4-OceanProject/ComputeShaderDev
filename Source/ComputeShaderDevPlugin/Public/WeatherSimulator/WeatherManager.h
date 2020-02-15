#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"
#include "RenderCommandFence.h"
#include "DynamicRHIResourceArray.h" // Core module
#include "RenderCommandFence.h" // RenderCore module
#include "WeatherManager.generated.h"


//An actor based weather system for simulating weather
UCLASS(Blueprintable, BlueprintType)
class AWeatherManager : public AWeatherManager_Properties
{
	GENERATED_UCLASS_BODY()

public:
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaSeconds) override;

	//This is manually called in blueprints, everything else is automatically handled
	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		void Compute(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool UploadResourceParametersToShader(
			/*  input */
			const TArray<FStruct_Cell_CPU>& gridSizeK_,
			const TArray<FStruct_GroundGridContainer_CPU>& ground_,
			const TArray<FStruct_AirGridContainer_CPU>& gridRslow_,
			const TArray<FStruct_AirGridContainer_CPU>& gridInit_,
			const TArray<FStruct_AirGridContainer_CPU>& grid3D_
		);

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		void SetUniformBuffersInShader(
			/*  input */const float y, const float z);

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Calculate(
			/*  input */const float x,
			/* input */UPARAM(ref) TArray<FStruct_AirGridContainer_CPU>& input,
			/* output */TArray<FStruct_AirGridContainer_CPU>& output);

protected:
	//This runs on the game thread
	void ExecuteComputeShader(TArray<FStruct_AirGridContainer_CPU> &currentStates, float DeltaTime);
	//This runs on the render thread
	void ExecuteInRenderThread(TArray<FStruct_AirGridContainer_CPU> &currentStates);

private:
	int32 num_input_ = 2;
	//Store the number of rows in array for each array
	int32 gridSizeK_num_input_ = 0;
	int32 ground_num_input_ = 0;
	int32 gridRslow_num_input_ = 0;
	int32 gridInit_num_input_ = 0;
	int32 grid3D_num_input_ = 0;

	FVector offset_;

	//Fencing forces the game thread to wait for the render thread to finish
	FRenderCommandFence render_command_fence_; // Necessary for waiting until a render command function finishes.

	const TShaderMap<FGlobalShaderType>* shader_map = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	//TResourceArray<FStruct_Cell_CPU> FStruct_Cell_gridSizeK_CPU_ResourceParameter_RA_;
	//FRHIResourceCreateInfo FStruct_Cell_gridSizeK_CPU_ResourceParameter_resource_;
	//FStructuredBufferRHIRef FStruct_Cell_gridSizeK_CPU_ResourceParameter_buffer_;
	//FShaderResourceViewRHIRef FStruct_Cell_gridSizeK_CPU_ResourceParameter_SRV_;

	//TResourceArray<FStruct_GroundGridContainer_CPU> FStruct_GroundGridContainer_ground_CPU_ResourceParameter_RA_;
	//FRHIResourceCreateInfo FStruct_GroundGridContainer_ground_CPU_ResourceParameter_resource_;
	//FStructuredBufferRHIRef FStruct_GroundGridContainer_ground_CPU_ResourceParameter_buffer_;
	//FShaderResourceViewRHIRef FStruct_GroundGridContainer_ground_CPU_ResourceParameter_SRV_;

	//TResourceArray<FStruct_AirGridContainer_CPU> FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_RA_;
	//FRHIResourceCreateInfo FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_resource_;
	//FStructuredBufferRHIRef FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_buffer_;
	//FShaderResourceViewRHIRef FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_SRV_;

	//TResourceArray<FStruct_AirGridContainer_CPU> FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_RA_;
	//FRHIResourceCreateInfo FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_resource_;
	//FStructuredBufferRHIRef FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_buffer_;
	//FShaderResourceViewRHIRef FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_SRV_;     //Issues with these when end play

	TResourceArray<FWarpInConfig2> output_RA_; // Not necessary.
	FRHIResourceCreateInfo output_resource_;
	FStructuredBufferRHIRef output_buffer_;
	FUnorderedAccessViewRHIRef output_UAV_;

	void SetUniformBuffersInShader_RenderThread(
		/*  input */const float y, const float z);

	void Calculate_RenderThread(
		/*  input */const FVector xyz, const bool yz_updated,
		/* output */TArray<FStruct_AirGridContainer_CPU>* output);

	void PrintResult(const TArray<FStruct_AirGridContainer_CPU>& output);
};