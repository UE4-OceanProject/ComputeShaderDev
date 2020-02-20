#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h" //ShaderCore module
#include "UniformBuffer.h" // RenderCore module
#include "RHICommandList.h" // RHI module
#include "Shader_Interface.h"
#include "ShaderParameterUtils.h"
#include "Shader_Interface_Parameters.h"
#include "WeatherStructs.h"
#include "CoreMinimal.h"
#include "GlobalShader.h" //ShaderCore module
#include "UniformBuffer.h" // RenderCore module
#include "RHICommandList.h" // RHI module

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "RenderCommandFence.h"

#include "ShaderPrintParameters.h"
#include "RenderGraphUtils.h"
#include "Containers/DynamicRHIResourceArray.h" // Core module

//Change this back when fix is in place
	//	//https://github.com/EpicGames/UnrealEngine/blob/e7ca4c774a649afc6d5c06f2ca0b22b3e3cf92b1/Engine/Source/Runtime/RHI/Public/RHIDefinitions.h#L1650 <-UBMT_UAV not available in binary
	//BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FGlobalWeatherParameters, )
	//	SHADER_PARAMETER_UAV(RWStructuredBuffer<FWarpInConfig2>, test_outputA) //<--Due to bug in engine we have to wait
	//END_GLOBAL_SHADER_PARAMETER_STRUCT()


/*****************************************************************************/
/* This class is what encapsulates the shader in the engine.                 */
/* It is the main bridge between the HLSL (.usf file) and the engine itself. */
/*****************************************************************************/
class /*COMPUTESHADERTEST419_API*/ FGlobalComputeShader_Interface : public FGlobalShader {
public:


	DECLARE_GLOBAL_SHADER(FGlobalComputeShader_Interface);

	FGlobalComputeShader_Interface() {}//Change this back when fix is in place
	explicit FGlobalComputeShader_Interface(const ShaderMetaType::CompiledShaderInitializerType& initializer); //Change this back when fix is in place

	//SHADER_USE_PARAMETER_STRUCT(FGlobalComputeShader_Interface, FGlobalShader);//Change this back when fix is in place

	//Change this back when fix is in place
	//BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
	//	SHADER_PARAMETER_UAV(RWStructuredBuffer<FWarpInConfig2>, test_outputA) //Change this back when fix is in place
	//	//SHADER_PARAMETER_STRUCT_REF(FGlobalWeatherParameters, GlobalWeatherParameters) //Change this back when fix is in place
	//END_SHADER_PARAMETER_STRUCT()


		virtual bool Serialize(FArchive& Ar) override;//Change this back when fix is in place

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& PermutationParams) {
		return true;
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

	void SetParameters();

	void Compute();

	void Calculate_RenderThread();

	void Setup_RenderThread();

	FRenderCommandFence render_command_fence_; // Necessary for waiting until a render command function finishes.

	int num_input_ = 2;

	FShaderResourceParameter FStruct_AirGridContainer_gridInit_CPU_ResourceParameter; //x5600  //Change this back when fix is in place
	FShaderResourceParameter A_output_; // RWStructuredBuffer<float3> test_outputA;  //Change this back when fix is in place
	TResourceArray<FWarpInConfig2> A_output_RA_; // Not necessary.
	FRHIResourceCreateInfo A_output_resource_;
	FStructuredBufferRHIRef A_output_buffer_;
	FUnorderedAccessViewRHIRef A_output_UAV_;
};
