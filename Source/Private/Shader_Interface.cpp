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

namespace WeatherShader_Interface 
{

	// ShaderPrint uniform buffer
	IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FWeatherUniformBufferParameters, "ShaderPrintUniform");
	typedef TUniformBufferRef<FWeatherUniformBufferParameters> FUniformBufferRef;


	/*****************************************************************************/
	/* This class is what encapsulates the shader in the engine.                 */
	/* It is the main bridge between the HLSL (.usf file) and the engine itself. */
	/*****************************************************************************/
	class /*COMPUTESHADERTEST419_API*/ FGlobalComputeShader_Interface : public FGlobalShader {
	public:
		//DECLARE_SHADER_TYPE(FGlobalComputeShader_Interface, Global, );
		DECLARE_GLOBAL_SHADER(FGlobalComputeShader_Interface);
		SHADER_USE_PARAMETER_STRUCT(FGlobalComputeShader_Interface, FGlobalShader);

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

		const TShaderMap<FGlobalShaderType>* shader_map = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	};

	//Declare our shader:   ShaderClassType					ShaderFilePath										    Shader function name		Type
	IMPLEMENT_GLOBAL_SHADER(FGlobalComputeShader_Interface, "/Plugin/ComputeShaderDev/Private/WeatherShader.usf",      "simulateStep",       SF_Compute);

}