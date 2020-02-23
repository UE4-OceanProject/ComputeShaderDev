#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h" //ShaderCore module
#include "UniformBuffer.h" // RenderCore module
#include "RHICommandList.h" // RHI module
#include "Shader_Interface.h"
#include "WeatherStructs.h"

#include "Shader.h"
#include "ShaderParameters.h"
#include "RenderCommandFence.h"

#include "RenderGraphUtils.h"
#include "Containers/DynamicRHIResourceArray.h" // Core module

#include "ShaderPrintParameters.h"



///*****************************************************************************/
///* This class is what encapsulates the shader in the engine.                 */
///* It is the main bridge between the HLSL (.usf file) and the engine itself. */
///*****************************************************************************/
class /*COMPUTESHADERTEST419_API*/ FGlobalComputeShader_Interface : public FGlobalShader {
	DECLARE_GLOBAL_SHADER(FGlobalComputeShader_Interface)
	SHADER_USE_PARAMETER_STRUCT(FGlobalComputeShader_Interface, FGlobalShader)

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		//SHADER_PARAMETER_RDG_TEXTURE_UAV(RWStructuredBuffer<FWarpInConfig2>, test_outputA)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<FWarpInConfig2>, test_outputA) //<--Due to bug in engine we have to wait
		//SHADER_PARAMETER_STRUCT_INCLUDE(ShaderPrint_Custom::FShaderParameters, ShaderPrintUniformBuffer)
		END_SHADER_PARAMETER_STRUCT()

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

		// Add your own defines for the shader code
		//OutEnvironment.SetDefine(TEXT("MY_DEFINE"), 1);
		//OutEnvironment.SetDefine(TEXT("NAME"), TEXT("Test"));
	}

	static bool ShouldCache(EShaderPlatform platform) {
		// Could skip compiling if the platform does not support DirectX Shader Model 5, for example, like the following.
		return IsFeatureLevelSupported(platform, ERHIFeatureLevel::SM5);
	}

public:
	TResourceArray<FWarpInConfig2> A_output_RA_; // Not necessary.
	FRHIResourceCreateInfo A_output_resource_;
	FStructuredBufferRHIRef A_output_buffer_;
	FUnorderedAccessViewRHIRef A_output_UAV_;

	void SetParameters(FRHICommandListImmediate& RHICmdList);

	void Compute(FRHICommandListImmediate& RHICmdList);
};

