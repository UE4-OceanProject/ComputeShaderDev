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


//FShaderParameter dTParameter;
//FShaderParameter gridXParameter;
//FShaderParameter gridYParameter;
//FShaderParameter gridZParameter;
//FShaderParameter gridSizeIParameter;
//FShaderParameter gridSizeJParameter;
//FShaderParameter simulationTimeParameter;
//FShaderParameter prevGCParameter;
//FShaderParameter currGCParameter;
//FShaderParameter nextGCParameter;
//
////FShaderResourceParameter input_position_; // StructuredBuffer<float3> test_input_position;
////FShaderResourceParameter input_scalar_; // StructuredBuffer<float> test_input_scalar;
//FShaderResourceParameter FStruct_Cell_gridSizeK_CPU_ResourceParameter; //single stack of values
//FShaderResourceParameter FStruct_GroundGridContainer_ground_CPU_ResourceParameter; //100
//FShaderResourceParameter FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter; //x5600
//FShaderResourceParameter FStruct_AirGridContainer_gridInit_CPU_ResourceParameter; //x5600
//
//
//
////THIS IS OUTPUT
//FShaderResourceParameter A_output_; // RWStructuredBuffer<float3> test_outputA;
//FShaderResourceParameter B_output_; // RWStructuredBuffer<float3> test_outputB;
//FShaderResourceParameter C_output_; // RWStructuredBuffer<float3> test_outputC;

//class FArchive;
//class FRHICommandListImmediate;
//class FShaderParameterMap;
//class FViewInfo;

/*****************************************************************************/
/* This class is what encapsulates the shader in the engine.                 */
/* It is the main bridge between the HLSL (.usf file) and the engine itself. */
/*****************************************************************************/
class /*COMPUTESHADERTEST419_API*/ FGlobalComputeShader_Interface : public FGlobalShader {
public:




	//DECLARE_SHADER_TYPE(FGlobalComputeShader_Interface, Global, );
	DECLARE_GLOBAL_SHADER(FGlobalComputeShader_Interface);
	SHADER_USE_PARAMETER_STRUCT(FGlobalComputeShader_Interface, FGlobalShader);

	// ShaderPrint uniform buffer layout
	BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FWeatherUniformBufferParameters, )
		SHADER_PARAMETER_ARRAY(float, Data, [2])
		END_GLOBAL_SHADER_PARAMETER_STRUCT()

		// ShaderPrint parameter struct declaration
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		//SHADER_PARAMETER(TArray<FWarpInConfig2>, Data)
		//SHADER_PARAMETER_STRUCT_REF(FWeatherUniformBufferParameters, UniformBufferParameters)
		//SHADER_PARAMETER_UAV(RWStructuredBuffer<ShaderPrintItem>, RWValuesBuffer)
		//SHADER_PARAMETER_STRUCT_INCLUDE(ShaderPrint::FShaderParameters, ShaderPrintUniformBuffer)
		SHADER_PARAMETER_UAV(RWTexture3D<FWarpInConfig2>, test_outputA)
		END_SHADER_PARAMETER_STRUCT()

		//explicit FGlobalComputeShader_Interface(const ShaderMetaType::CompiledShaderInitializerType& initializer);

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& PermutationParams) {
		// Useful when adding a permutation of a particular shader
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

	//const TShaderMap<FGlobalShaderType>* shader_map = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	//TShaderMapRef<FGlobalComputeShader_Interface>* ComputeShader(shader_map);

			// Call this to fill the FShaderParameters
	void SetParameters(FViewInfo const& View, FParameters& OutParameters);






};
