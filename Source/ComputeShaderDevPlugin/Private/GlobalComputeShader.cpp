//Here we setup:
//The output surface the shader will use in .usf
//The buffers that the shader cann access in .usf
//Which shader (.usf) we want to give these to
//And what kind of .usf shader this is (compute)

#include "GlobalComputeShader.h"

//Declare our plugin module we want to reference
IMPLEMENT_MODULE(FDefaultModuleImpl, ComputeShaderDevPlugin)

//Declare our shader:   ShaderType						ShaderFileName													Shader function name		Type
IMPLEMENT_SHADER_TYPE(, FGlobalComputeShader, TEXT("/Plugin/ComputeShaderDevPlugin/Private/ComputeShader.usf"),	TEXT("VS_test"),	SF_Compute);

//Define the name of the buffer structs we use in the shader itself
//They also need to be unique over the entire solution since they can in fact be accessed from any shader
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FConstantParameters, TEXT("constants"))
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FVariableParameters, TEXT("variables"))

//Set the names of the OutputSurface the WeatherComputeShader can use
void FGlobalComputeShader::SetUniformBuffers(FRHICommandList& RHICmdList, FConstantParameters& constants, FVariableParameters& variables)
{
	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FConstantParameters>(),
		FConstantParametersRef::CreateUniformBufferImmediate(constants, UniformBuffer_SingleDraw));
	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FVariableParameters>(),
		FVariableParametersRef::CreateUniformBufferImmediate(variables, UniformBuffer_SingleDraw));
}

//Here we bind or "map" our c++ struct to the shader struct
FGlobalComputeShader::FGlobalComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	TArray_Struct_Data.Bind(Initializer.ParameterMap, TEXT("TArray_Struct_Data"));
}

//Sets the OutputSurface the WeatherComputeShader can use
void FGlobalComputeShader::SetSurfaces(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef uav)
{
	if (TArray_Struct_Data.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Data.GetBaseIndex(), uav);
}

void FGlobalComputeShader::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

/* Unbinds buffers that will be used elsewhere */
void FGlobalComputeShader::UnbindBuffers(FRHICommandList& RHICmdList)
{
	if (TArray_Struct_Data.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Data.GetBaseIndex(), FUnorderedAccessViewRHIRef());
}
