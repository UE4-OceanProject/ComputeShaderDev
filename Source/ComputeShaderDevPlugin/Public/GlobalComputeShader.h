// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "ShaderParameterUtils.h"

//Declare the variables inside of our struct
//This FStruct_Shader_GPU_Buffer should contain variables that never, or rarely change
BEGIN_UNIFORM_BUFFER_STRUCT(FConstantParameters_CPU, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(int, ArrayNum)
END_UNIFORM_BUFFER_STRUCT(FConstantParameters_CPU)

//Declare the variables inside of our struct
//This FStruct_Shader_GPU_Buffer is for variables that change very often (each frame for example)
BEGIN_UNIFORM_BUFFER_STRUCT(FVariableParameters_CPU, )
DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, DeltaTime)
END_UNIFORM_BUFFER_STRUCT(FVariableParameters_CPU)

typedef TUniformBufferRef<FConstantParameters_CPU> FConstantParametersRef;
typedef TUniformBufferRef<FVariableParameters_CPU> FVariableParametersRef;

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

	//Declares FShaderResourceParameters whos value can be set at runtime.
	FShaderParameter MyColorParameter;

	FShaderResourceParameter TArray_Struct_Parameter_CPU;
	//This is bound to the same data, but exposed to the shader as a UAV
	FShaderResourceParameter OutputSurface_Parameter_CPU;
		
	//Assign a name for each FShaderResourceParamteter for use in the .usf file
	explicit FGlobalComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		TArray_Struct_Parameter_CPU.Bind(Initializer.ParameterMap, TEXT("TArray_Struct_Parameter_GPU"), SPF_Mandatory);
		OutputSurface_Parameter_CPU.Bind(Initializer.ParameterMap, TEXT("OutputSurface_Parameter_GPU"), SPF_Optional);
		//MyColorParameter.Bind(Initializer.ParameterMap, TEXT("MyColor"), SPF_Optional);

		//InputSurface.Bind(Initializer.ParameterMap, TEXT("InputSurfaceCS"));
	}

	// FShader interface: tells ParameterMap which FShaderResourceParameters we want to bind
	//Updates cached ParameterMap if we add a new param here
	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);
		//Ar << YourResourceName1 << YourResourceName2 << YourResourceName3 << ......;
		Ar << TArray_Struct_Parameter_CPU << OutputSurface_Parameter_CPU << MyColorParameter;
		return bShaderHasOutdatedParams;
	}

	//Example on how to set a simple parameter before shader use
	void SetColor(FRHICommandList& RHICmdList, const FLinearColor& Color)
	{
		SetShaderValue(RHICmdList, GetPixelShader(), MyColorParameter, Color);
	}


	//We declared TArray_Struct_Parameter_CPU as TArray_Struct_Parameter_GPU,
	//now we bind the uav data to this!
	void BindDataInterfaceToShaderParamName(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef uav)
	{
		if (TArray_Struct_Parameter_CPU.IsBound()) //This should be bound to the TArray_Struct_Parameter_GPU name
			RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Parameter_CPU.GetBaseIndex(), uav); //bind uav data to TArray_Struct_Parameter_GPU name

	}

	//This function is required to bind our constant / uniform buffers to the shader.
	void BindDataInterfaceToUniformBuffersParamName(FRHICommandList& RHICmdList, FConstantParameters_CPU& constants, FVariableParameters_CPU& variables)
	{
			// the uniform FStruct_Shader_GPU_Buffer is temporary, used for a single draw call then discarded
			//UniformBuffer_SingleDraw = 0,
			// the uniform FStruct_Shader_GPU_Buffer is used for multiple draw calls but only for the current frame
			//UniformBuffer_SingleFrame,
			// the uniform FStruct_Shader_GPU_Buffer is used for multiple draw calls, possibly across multiple frames
			//UniformBuffer_MultiFrame,
		SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FConstantParameters_CPU>(),
			FConstantParametersRef::CreateUniformBufferImmediate(constants, UniformBuffer_SingleDraw));
		SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FVariableParameters_CPU>(),
			FVariableParametersRef::CreateUniformBufferImmediate(variables, UniformBuffer_SingleDraw));
	}

	//This is used to clean up the FStruct_Shader_GPU_Buffer binds after each invocation to let them be changed and used elsewhere if needed.
	void UnbindBuffers(FRHICommandList& RHICmdList)
	{
		if (TArray_Struct_Parameter_CPU.IsBound())
			RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Parameter_CPU.GetBaseIndex(), FUnorderedAccessViewRHIRef());
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

};