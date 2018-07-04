// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

//Declare the variables inside of our struct
//This buffer should contain variables that never, or rarely change
BEGIN_UNIFORM_BUFFER_STRUCT(FConstantParameters, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(int, ArrayNum)
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
	
	explicit FGlobalComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	//ShouldCompilePermutation and ShouldCache both need to return true, in order to be compiled for whatever platform
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		//if current platform = x, return true, else false
		//or
		//if current platform = x, return false (not compatible) else true
		return true;
	}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	}

	//Here you can modify compiliation flags and also modify Defines in the usf file before compiliation
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	}

	//UE4 uses the ParameterMap to manage all resources that declared in shaders
	//UE4 will read a cached version of your ParameterMap unless this function tells it to update
	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);
		//Ar << YourResourceName1 << YourResourceName2 << YourResourceName3 << ......;
		Ar << TArray_Struct_Data;
		return bShaderHasOutdatedParams;
	}

	//This function is required to let us bind our runtime surface to the shader using an UAV.
	//So we can read and write to it
	//Sets the OutputSurface the WeatherComputeShader can use
	void SetSurfaces(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef uav)
	{
		if (TArray_Struct_Data.IsBound())
			RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Data.GetBaseIndex(), uav);
	}


	//Set the names of the OutputSurface the WeatherComputeShader can use
	//This function is required to bind our constant / uniform buffers to the shader.
	void SetUniformBuffers(FRHICommandList& RHICmdList, FConstantParameters& constants, FVariableParameters& variables)
	{
		SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FConstantParameters>(),
			FConstantParametersRef::CreateUniformBufferImmediate(constants, UniformBuffer_SingleDraw));
		SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FVariableParameters>(),
			FVariableParametersRef::CreateUniformBufferImmediate(variables, UniformBuffer_SingleDraw));
	}

	/* Unbinds buffers that will be used elsewhere */
	//This is used to clean up the buffer binds after each invocation to let them be changed and used elsewhere if needed.
	void UnbindBuffers(FRHICommandList& RHICmdList)
	{
		if (TArray_Struct_Data.IsBound())
			RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Data.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}


private:
	//This is the actual output resource that we will bind to the compute shader
	FShaderResourceParameter TArray_Struct_Data;
};