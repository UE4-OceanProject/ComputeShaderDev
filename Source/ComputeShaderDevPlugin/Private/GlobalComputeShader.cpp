#include "GlobalComputeShader.h"

//Declare our plugin module we want to reference
IMPLEMENT_MODULE(FDefaultModuleImpl, ComputeShaderDevPlugin)

// This needs to go on a cpp file
//Declare our shader:   ShaderType						ShaderFileName													Shader function name		Type
IMPLEMENT_SHADER_TYPE(, FGlobalComputeShader, TEXT("/Plugin/ComputeShaderDevPlugin/Private/ComputeShader.usf"),	TEXT("VS_test"),	SF_Compute);

//Define the name of the FStruct_Shader_GPU_Buffer structs we use in the shader itself
//They also need to be unique over the entire solution since they can in fact be accessed from any shader
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FConstantParameters, TEXT("constants"))
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FVariableParameters, TEXT("variables"))
