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



void FGlobalComputeShader_Interface::SetParameters(FViewInfo const & View, FParameters & OutParameters)
	{
	}

	//Declare our shader:   ShaderClassType					ShaderFilePath										    Shader function name		Type
	IMPLEMENT_GLOBAL_SHADER(FGlobalComputeShader_Interface, "/Plugin/ComputeShaderDev/Private/WeatherShader.usf",      "simulateStep",       SF_Compute);


