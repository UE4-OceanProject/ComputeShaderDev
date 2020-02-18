// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

// The ShaderPrint system uses a RWBuffer to capture any debug print from a shader.
// This means that the buffer needs to be bound for the shader you wish to debug.
// It would be ideal if that was automatic (maybe by having a fixed bind point for the buffer and binding it for the entire view).
// But for now you need to manually add binding information to your FShader class.
// To do this either:
// (i) Use SHADER_PARAMETER_STRUCT_INCLUDE(ShaderPrint::FShaderParameters) in your FShader::FParameters declaration and call SetParameters().
// (ii) Put an FShaderParametersLegacy as a member of your FShader and add calls into Bind()/SetParameters().

// Also it seems that we can only bind a RWBuffer to compute shaders right now. Fixing this would allow us to use this system from all shader stages.

#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameters.h"

class FArchive;
class FRHICommandListImmediate;
class FShaderParameterMap;
class FViewInfo;

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

namespace WeatherShader_Interface
{
	// ShaderPrint uniform buffer layout
	BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FWeatherUniformBufferParameters, )
		SHADER_PARAMETER(FVector4, FontSize)
		SHADER_PARAMETER(int32, MaxValueCount)
		SHADER_PARAMETER(int32, MaxSymbolCount)
		END_GLOBAL_SHADER_PARAMETER_STRUCT()

		// ShaderPrint parameter struct declaration
		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FWeatherUniformBufferParameters, UniformBufferParameters)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<ShaderPrintItem>, RWValuesBuffer)
		//SHADER_PARAMETER_STRUCT_INCLUDE(ShaderPrint::FShaderParameters, ShaderPrintUniformBuffer)
		END_SHADER_PARAMETER_STRUCT()

		// Call this to fill the FShaderParameters
		void SetParameters(FViewInfo const& View, FParameters& OutParameters);

}
