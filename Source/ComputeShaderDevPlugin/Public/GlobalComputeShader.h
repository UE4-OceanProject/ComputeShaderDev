// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

//Declare the variables inside of our struct
//This buffer should contain variables that never, or rarely change
BEGIN_UNIFORM_BUFFER_STRUCT(FConstantParameters, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(int, WeatherPointsCount)
END_UNIFORM_BUFFER_STRUCT(FConstantParameters)

//Declare the variables inside of our struct
//This buffer is for variables that change very often (each frame for example)
BEGIN_UNIFORM_BUFFER_STRUCT(FVariableParameters, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, DeltaTime)
END_UNIFORM_BUFFER_STRUCT(FVariableParameters)

typedef TUniformBufferRef<FConstantParameters> FConstantParametersRef;
typedef TUniformBufferRef<FVariableParameters> FVariableParametersRef;

/*****************************************************************************/
/* This class is what encapsulates the shader in the engine.                 */
/* It is the main bridge between the HLSL (.usf file) and the engine itself. */
/*****************************************************************************/
class FGlobalComputeShader : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FGlobalComputeShader, Global);

public:

	FGlobalComputeShader() {}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		// XXX Biggs ???
		return true;
	}
	
	explicit FGlobalComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCache(EShaderPlatform Platform) { return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5); }

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	virtual bool Serialize(FArchive& Ar) override { bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar); Ar << CS_ShaderResourceDataStruct; return bShaderHasOutdatedParams; }

	//This function is required to let us bind our runtime surface to the shader using an UAV.
	void SetSurfaces(FRHICommandList& commandList, FUnorderedAccessViewRHIRef uav);
	//This function is required to bind our constant / uniform buffers to the shader.
	void SetUniformBuffers(FRHICommandList& commandList, FConstantParameters& constants, FVariableParameters& variables);
	//This is used to clean up the buffer binds after each invocation to let them be changed and used elsewhere if needed.
	void UnbindBuffers(FRHICommandList& commandList);

private:
	//This is the actual output resource that we will bind to the compute shader
	FShaderResourceParameter CS_ShaderResourceDataStruct;
};