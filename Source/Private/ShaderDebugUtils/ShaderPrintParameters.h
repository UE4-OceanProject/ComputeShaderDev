// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

// The ShaderPrint system uses a RWBuffer to capture any debug print from a shader.
// This means that the buffer needs to be bound for the shader you wish to debug.
// It would be ideal if that was automatic (maybe by having a fixed bind point for the buffer and binding it for the entire view).
// But for now you need to manually add binding information to your FShader class.
// To do this either:
// (i) Use SHADER_PARAMETER_STRUCT_INCLUDE(ShaderPrint_Custom::FShaderParameters) in your FShader::FParameters declaration and call SetParameters().

// Also it seems that we can only bind a RWBuffer to compute shaders right now. Fixing this would allow us to use this system from all shader stages.

#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameters.h"

class FArchive;
class FRHICommandListImmediate;
class FShaderParameterMap;
//class FViewInfo;

namespace ShaderPrint_Custom
{
	// ShaderPrint uniform buffer layout
	BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FShaderPrintUniformBufferParameters, )
		SHADER_PARAMETER(FVector4, FontSize)
		SHADER_PARAMETER(int32, MaxValueCount)
		SHADER_PARAMETER(int32, MaxSymbolCount)
	END_GLOBAL_SHADER_PARAMETER_STRUCT()

	// ShaderPrint parameter struct declaration
	BEGIN_SHADER_PARAMETER_STRUCT(FShaderParameters, )
		SHADER_PARAMETER_STRUCT_REF(FShaderPrintUniformBufferParameters, UniformBufferParameters)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<ShaderPrintItem>, RWValuesBuffer)
	END_SHADER_PARAMETER_STRUCT()

	// Call this to fill the FShaderParameters
	void SetParameters(FShaderParameters& OutParameters);



}
