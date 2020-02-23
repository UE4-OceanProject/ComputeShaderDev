#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h" //ShaderCore module
#include "Shader_Interface.h"
#include "WeatherStructs.h"




#include "RenderGraph.h" //The only thing you need for RDG
#include "ShaderPrintParameters.h"



///*****************************************************************************/
///* This class is what encapsulates the shader in the engine.                 */
///* It is the main bridge between the HLSL (.usf file) and the engine itself. */
///*****************************************************************************/
class /*COMPUTESHADERTEST419_API*/ FGlobalComputeShader_Interface : public FGlobalShader {
	DECLARE_GLOBAL_SHADER(FGlobalComputeShader_Interface)
	SHADER_USE_PARAMETER_STRUCT(FGlobalComputeShader_Interface, FGlobalShader)

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FWarpInConfig2>, test_outputA)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FWarpInConfig2>, test_outputB)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FWarpInConfig2>, test_outputC)
		END_SHADER_PARAMETER_STRUCT()

		//Don't compile for a platform we don't support.
		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
		{
			return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
		}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& PermutationParams, FShaderCompilerEnvironment& OutEnvironment) {
		FGlobalShader::ModifyCompilationEnvironment(PermutationParams, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
		//OutEnvironment.CompilerFlags.Add(CFLAG_OnChip);
		OutEnvironment.CompilerFlags.Add(CFLAG_PreferFlowControl);
		OutEnvironment.CompilerFlags.Add(CFLAG_Debug);
		OutEnvironment.CompilerFlags.Add(CFLAG_KeepDebugInfo);
	}

	static bool ShouldCache(EShaderPlatform platform) {
		// Could skip compiling if the platform does not support DirectX Shader Model 5, for example, like the following.
		return IsFeatureLevelSupported(platform, ERHIFeatureLevel::SM5);
	}

public:

	//We have 3 grids, past, current, future. The simulation rotates these around as needed to prevent copying
	int buffer_index_rotator = 1;
	TArray<FUnorderedAccessViewRHIRef>RotateableBufers;

	//This is a reference to our data on the GPU, without it, we would need to pass the entire buffer to the GPU for our next itteration
	FUnorderedAccessViewRHIRef A_output_UAV_;
	FUnorderedAccessViewRHIRef B_output_UAV_;
	FUnorderedAccessViewRHIRef C_output_UAV_;

	//Send our data to the gpu, and do our first itteration
	void SetParameters(FRHICommandListImmediate& RHICmdList);

	//Data is already on GPU, do a single itteration
	void Compute(FRHICommandListImmediate& RHICmdList);
};

