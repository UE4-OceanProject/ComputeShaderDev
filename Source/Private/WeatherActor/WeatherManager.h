#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"

#include "Containers/DynamicRHIResourceArray.h" // Core module
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

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Setup();

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Calculate(
			/*  input */const float x,
			/* input */UPARAM(ref) TArray<FStruct_AirGridContainer_CPU>& input,
			/* output */TArray<FStruct_AirGridContainer_CPU>& output);

protected:
	//This runs on the game thread
	void Calculate_RenderThread(FGlobalComputeShader_Interface::FParameters Parameters);

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

	//TResourceArray<FWarpInConfig2> A_output_RA_; // Not necessary.
	//FRHIResourceCreateInfo A_output_resource_;
	//FStructuredBufferRHIRef A_output_buffer_;
	//FUnorderedAccessViewRHIRef A_output_UAV_;

	//TResourceArray<FWarpInConfig2> B_output_RA_; // Not necessary.
	//FRHIResourceCreateInfo B_output_resource_;
	//FStructuredBufferRHIRef B_output_buffer_;
	//FUnorderedAccessViewRHIRef B_output_UAV_;

	//TResourceArray<FWarpInConfig2> C_output_RA_; // Not necessary.
	//FRHIResourceCreateInfo C_output_resource_;
	//FStructuredBufferRHIRef C_output_buffer_;
	//FUnorderedAccessViewRHIRef C_output_UAV_;

	void PrintResult(const TArray<FStruct_AirGridContainer_CPU>& output);

};