#include "Shader_Interface.h"

// This needs to go on a cpp file
//Declare our shader:   ShaderClassType						ShaderFileName													Shader function name		Type
IMPLEMENT_SHADER_TYPE(, FGlobalComputeShader, TEXT("/Plugin/ComputeShaderDev/Private/WeatherShader.usf"),	TEXT("simulateStep"),	SF_Compute);


