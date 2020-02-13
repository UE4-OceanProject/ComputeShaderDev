#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"
#include "RenderCommandFence.h"
#include "DynamicRHIResourceArray.h" // Core module
#include "RenderCommandFence.h" // RenderCore module
#include "WeatherManager.generated.h"


//My Struct used to be here


//An actor based weather system for simulating weather
UCLASS(Blueprintable, BlueprintType)
class AWeatherManager : public AWeatherManager_Properties
{
	GENERATED_UCLASS_BODY()

public:

	// Don't need this is using GENERATED_UCLASS_BODY() instead of GENERATED_BODY()... I think
	// Sets default values for this actor's properties
	//AWeatherManager();
	
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
			/* output */UPARAM(ref) TArray<FStruct_AirGridContainer_CPU>& grid3D_,
			/* output */TArray<FVector>& output);

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
	//Note:
	// GetWorld() function cannot be called from constructor, can be called after BeginPlay() instead.
	// So I used GMaxRHIFeatureLevel, instead of GetWorld()->Scene->GetFeatureLevel().
	//const TShaderMap<FTestComputeShader::ShaderMetaType>* shader_map = GetGlobalShaderMap(GetWorld()->Scene->GetFeatureLevel());

	//// Get the actual shader instance off the ShaderMap
	//TShaderMapRef<FTestComputeShader> test_compute_shader_{ shader_map }; // Note: test_compute_shader_(shader_map) causes error.

	//TResourceArray<FVector> input_positions_RA_;
	//FRHIResourceCreateInfo input_positions_resource_;
	//FStructuredBufferRHIRef input_positions_buffer_;
	//FShaderResourceViewRHIRef input_positions_SRV_;

	//TResourceArray<float> input_scalars_RA_;
	//FRHIResourceCreateInfo input_scalars_resource_;
	//FStructuredBufferRHIRef input_scalars_buffer_;
	//FShaderResourceViewRHIRef input_scalars_SRV_;

	//TResourceArray<FVector> output_RA_; // Not necessary.
	//FRHIResourceCreateInfo output_resource_;
	//FStructuredBufferRHIRef output_buffer_;
	//FUnorderedAccessViewRHIRef output_UAV_;

	TResourceArray<FStruct_Cell_CPU> FStruct_Cell_gridSizeK_CPU_ResourceParameter_RA_;
	FRHIResourceCreateInfo FStruct_Cell_gridSizeK_CPU_ResourceParameter_resource_;
	FStructuredBufferRHIRef FStruct_Cell_gridSizeK_CPU_ResourceParameter_buffer_;
	FShaderResourceViewRHIRef FStruct_Cell_gridSizeK_CPU_ResourceParameter_SRV_;

	TResourceArray<FStruct_GroundGridContainer_CPU> FStruct_GroundGridContainer_ground_CPU_ResourceParameter_RA_;
	FRHIResourceCreateInfo FStruct_GroundGridContainer_ground_CPU_ResourceParameter_resource_;
	FStructuredBufferRHIRef FStruct_GroundGridContainer_ground_CPU_ResourceParameter_buffer_;
	FShaderResourceViewRHIRef FStruct_GroundGridContainer_ground_CPU_ResourceParameter_SRV_;

	TResourceArray<FStruct_AirGridContainer_CPU> FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_RA_;
	FRHIResourceCreateInfo FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_resource_;
	FStructuredBufferRHIRef FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_buffer_;
	FShaderResourceViewRHIRef FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_SRV_;

	TResourceArray<FStruct_AirGridContainer_CPU> FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_RA_;
	FRHIResourceCreateInfo FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_resource_;
	FStructuredBufferRHIRef FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_buffer_;
	FShaderResourceViewRHIRef FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_SRV_;

	TResourceArray<FVector> output_RA_; // Not necessary.
	FRHIResourceCreateInfo output_resource_;
	FStructuredBufferRHIRef output_buffer_;
	FUnorderedAccessViewRHIRef output_UAV_;


	TResourceArray<FStruct_AirGridContainer_CPU> FStruct_AirGridContainer_grid3D_CPU_ResourceParameter_RA_; // Not necessary.
	FRHIResourceCreateInfo FStruct_AirGridContainer_grid3D_CPU_ResourceParameter_resource_;
	FStructuredBufferRHIRef FStruct_AirGridContainer_grid3D_CPU_ResourceParameter_buffer_;
	FUnorderedAccessViewRHIRef FStruct_AirGridContainer_grid3D_CPU_ResourceParameter_UAV_;

	void SetUniformBuffersInShader_RenderThread(
		/*  input */const float y, const float z);

	void Calculate_RenderThread(
		/*  input */const FVector xyz, const bool yz_updated,
		/* output */TArray<FStruct_AirGridContainer_CPU>* grid3D_,
		/* output */TArray<FVector>* output);

	void PrintResult(const TArray<FVector>& output);
};