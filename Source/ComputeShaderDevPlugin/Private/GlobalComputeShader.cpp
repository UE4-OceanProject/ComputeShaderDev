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

//Here we bind or "map" our c++ struct to the shader struct
FGlobalComputeShader::FGlobalComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	TArray_Struct_Data.Bind(Initializer.ParameterMap, TEXT("TArray_Struct_Data"));
}


