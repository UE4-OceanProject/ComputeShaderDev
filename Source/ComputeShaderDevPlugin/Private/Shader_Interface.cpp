#include "Shader_Interface.h"

//Declare our plugin module we want to reference
IMPLEMENT_MODULE(FDefaultModuleImpl, ComputeShaderDevPlugin)

// This needs to go on a cpp file
//Declare our shader:   ShaderType						ShaderFileName													Shader function name		Type
IMPLEMENT_SHADER_TYPE(, FGlobalComputeShader, TEXT("/Plugin/ComputeShaderDevPlugin/Private/WeatherShader.usf"),	TEXT("simulateStep"),	SF_Compute);

