//Here we setup:
//The output surface the shader will use in .usf
//The buffers that the shader cann access in .usf
//Which shader (.usf) we want to give these to
//And what kind of .usf shader this is (compute)

#include "ComputeShaderDeclarations.h"

//Declare our plugin module we want to reference
IMPLEMENT_MODULE(FDefaultModuleImpl, ComputeShaderDevPlugin)

//Declare our shader:   ShaderType						ShaderFileName													Shader function name		Type
IMPLEMENT_SHADER_TYPE(, FComputeShaderDevPluginModule, TEXT("/Plugin/ComputeShaderDevPlugin/Private/ComputeShader.usf"),	TEXT("VS_test"),	SF_Compute);

//Define the name of the buffer structs we use in the shader itself
//They also need to be unique over the entire solution since they can in fact be accessed from any shader
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FConstantParameters, TEXT("constants"))
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FVariableParameters, TEXT("variables"))

//Set the names of the OutputSurface the WeatherComputeShader can use
void FComputeShaderDevPluginModule::SetUniformBuffers(FRHICommandList& RHICmdList, FConstantParameters& constants, FVariableParameters& variables)
{
	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FConstantParameters>(),
		FConstantParametersRef::CreateUniformBufferImmediate(constants, UniformBuffer_SingleDraw));
	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FVariableParameters>(),
		FVariableParametersRef::CreateUniformBufferImmediate(variables, UniformBuffer_SingleDraw));
}

//Here we bind or "map" our c++ struct to the shader struct
FComputeShaderDevPluginModule::FComputeShaderDevPluginModule(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	CS_ShaderResourceDataStruct.Bind(Initializer.ParameterMap, TEXT("data"));
}

//Sets the OutputSurface the WeatherComputeShader can use
void FComputeShaderDevPluginModule::SetSurfaces(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef uav)
{
	if (CS_ShaderResourceDataStruct.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), CS_ShaderResourceDataStruct.GetBaseIndex(), uav);
}

void FComputeShaderDevPluginModule::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

/* Unbinds buffers that will be used elsewhere */
void FComputeShaderDevPluginModule::UnbindBuffers(FRHICommandList& RHICmdList)
{
	if (CS_ShaderResourceDataStruct.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), CS_ShaderResourceDataStruct.GetBaseIndex(), FUnorderedAccessViewRHIRef());
}
