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
	//Not exporting to an external module
	//DECLARE_EXPORTED_SHADER_TYPE(FGlobalComputeShader, Global, SHADERCONTROLLER_API);

	FGlobalComputeShader() {}

	//This will be used in the Serialize function which keeps the ParameterMap updated
	//holds information for the runtime to be able to find the bindings, allowing the value of the parameter to be set at runtime.
	FShaderResourceParameter TArray_Struct_Parameter_CPU;
	//This is bound to the same data, but exposed to the shader as a UAV
	FShaderResourceParameter OutputSurface_CPU;
		
	//Assign a name for each FShaderResourceParamteter for use in the .usf file
	explicit FGlobalComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		TArray_Struct_Parameter_CPU.Bind(Initializer.ParameterMap, TEXT("TArray_Struct_Parameter_GPU"));
		OutputSurface_CPU.Bind(Initializer.ParameterMap, TEXT("OutputSurface_GPU"));
		//InputSurface.Bind(Initializer.ParameterMap, TEXT("InputSurfaceCS"));
	}

	// FShader interface: tells ParameterMap which FShaderResourceParameters we want to bind
	//Updates cached ParameterMap if we add a new param here
	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);
		//Ar << YourResourceName1 << YourResourceName2 << YourResourceName3 << ......;
		Ar << TArray_Struct_Parameter_CPU << OutputSurface_CPU;
		return bShaderHasOutdatedParams;
	}

	//ShouldCompilePermutation and ShouldCache both need to return true, in order to be compiled for whatever platform/permutation
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		// Useful when adding a permutation of a particular shader
		return true;
	}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		// Could skip compiling for Platform == SP_METAL for example
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	}

	//Here you can modify compiliation flags and also modify Defines in the usf file before compiliation
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
		// Add your own defines for the shader code
		//OutEnvironment.SetDefine(TEXT("MY_DEFINE"), 1);

	}

	//This function is required to let us bind our runtime surface to the shader using an UAV.
	//UAV is RW
	void SetSurfaces(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef uav)
	{
		if (TArray_Struct_Parameter_CPU.IsBound())
			RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Parameter_CPU.GetBaseIndex(), uav);
	}

	//This function is required to bind our constant / uniform buffers to the shader.
	void SetUniformBuffers(FRHICommandList& RHICmdList, FConstantParameters& constants, FVariableParameters& variables)
	{
		SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FConstantParameters>(),
			FConstantParametersRef::CreateUniformBufferImmediate(constants, UniformBuffer_SingleDraw));
		SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FVariableParameters>(),
			FVariableParametersRef::CreateUniformBufferImmediate(variables, UniformBuffer_SingleDraw));
	}

	//This is used to clean up the buffer binds after each invocation to let them be changed and used elsewhere if needed.
	void UnbindBuffers(FRHICommandList& RHICmdList)
	{
		if (TArray_Struct_Parameter_CPU.IsBound())
			RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Parameter_CPU.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}

};