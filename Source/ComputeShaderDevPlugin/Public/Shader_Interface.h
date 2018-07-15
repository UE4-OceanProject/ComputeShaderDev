// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "ShaderParameterUtils.h"

////Declare the variables inside of our struct
////This FStruct_Shader_GPU_Buffer should contain variables that never, or rarely change
//BEGIN_UNIFORM_BUFFER_STRUCT(FShaderConstants_Class, )
//UNIFORM_MEMBER(int, numSteps)
//UNIFORM_MEMBER(float, dT)
//UNIFORM_MEMBER(int, gridX)
//UNIFORM_MEMBER(int, gridY)
//UNIFORM_MEMBER(int, gridZ)
//UNIFORM_MEMBER(int, gridSizeI)
//UNIFORM_MEMBER(int, gridSizeJ)
//END_UNIFORM_BUFFER_STRUCT(FShaderConstants_Class)
//
////Declare the variables inside of our struct
////This FStruct_Shader_GPU_Buffer is for variables that change very often (each frame for example)
//BEGIN_UNIFORM_BUFFER_STRUCT(FVariables_Class, )
//UNIFORM_MEMBER(int, currInd)
//UNIFORM_MEMBER(float, simulationTime)
//END_UNIFORM_BUFFER_STRUCT(FVariables_Class)

//#define UNIFORM_MEMBER_ARRAY_EX(MemberType,MemberName,ArrayDecl,Precision)
//#define UNIFORM_MEMBER_ARRAY(MemberType,MemberName,ArrayDecl)
//#define UNIFORM_MEMBER(MemberType,MemberName)
//#define UNIFORM_MEMBER_EX(MemberType,MemberName,Precision)
//#define UNIFORM_MEMBER_SRV(ShaderType,MemberName)
// NOT SUPPORTED YET
//#define DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER_UAV(ShaderType,MemberName)
//#define UNIFORM_MEMBER_SAMPLER(ShaderType,MemberName)
//#define UNIFORM_MEMBER_TEXTURE(ShaderType,MemberName)
//#define UNIFORM_MEMBER_STRUCT(StructType,MemberName)


//typedef TUniformBufferRef<FShaderConstants_Class> FShaderConstants_ClassInstance;
//typedef TUniformBufferRef<FVariables_Class> FShaderVariables_ClassInstance;

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
	
	FShaderResourceParameter TArray_Struct_Parameter_CPU;
	//This is bound to the same data, but exposed to the shader as a UAV
	FShaderResourceParameter OutputSurface_Parameter_CPU;


	//These members are added to the FShaderParameter class, which will hold information for the runtime to be able
	//to find the bindings, allowing the value of the parameter to be set at runtime.
	//CAN BE UAV! 
	FShaderParameter numStepsParameter;
	FShaderParameter dTParameter;
	FShaderParameter gridXParameter;
	FShaderParameter gridYParameter;
	FShaderParameter gridZParameter;
	FShaderParameter gridSizeIParameter;
	FShaderParameter gridSizeJParameter;
	FShaderParameter currIndParameter;
	FShaderParameter simulationTimeParameter;

	//Example on how to set a simple parameter before shader use
	void SetShaderParameters(FRHICommandList& RHICmdList, int numSteps, float dT,
		int gridX, int gridY, int gridZ, int gridSizeI, int gridSizeJ,
		int currInd, float simulationTime)
	{
		SetShaderValue(RHICmdList, GetPixelShader(), numStepsParameter, numSteps);
		SetShaderValue(RHICmdList, GetPixelShader(), dTParameter, dT);
		SetShaderValue(RHICmdList, GetPixelShader(), gridXParameter, gridX);
		SetShaderValue(RHICmdList, GetPixelShader(), gridYParameter, gridY);
		SetShaderValue(RHICmdList, GetPixelShader(), gridZParameter, gridZ);
		SetShaderValue(RHICmdList, GetPixelShader(), gridSizeIParameter, gridSizeI);
		SetShaderValue(RHICmdList, GetPixelShader(), gridSizeJParameter, gridSizeJ);
		SetShaderValue(RHICmdList, GetPixelShader(), currIndParameter, currInd);
		SetShaderValue(RHICmdList, GetPixelShader(), simulationTimeParameter, simulationTime);
	}

	//Assign a name for each FShaderResourceParamteter for use in the .usf file
	explicit FGlobalComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		numStepsParameter.Bind(Initializer.ParameterMap, TEXT("numSteps"), SPF_Mandatory);
		dTParameter.Bind(Initializer.ParameterMap, TEXT("dT"), SPF_Mandatory);
		gridXParameter.Bind(Initializer.ParameterMap, TEXT("gridX"), SPF_Mandatory);
		gridYParameter.Bind(Initializer.ParameterMap, TEXT("gridY"), SPF_Mandatory);
		gridZParameter.Bind(Initializer.ParameterMap, TEXT("gridZ"), SPF_Mandatory);
		gridSizeIParameter.Bind(Initializer.ParameterMap, TEXT("gridSizeI"), SPF_Mandatory);
		gridSizeJParameter.Bind(Initializer.ParameterMap, TEXT("gridSizeJ"), SPF_Mandatory);
		currIndParameter.Bind(Initializer.ParameterMap, TEXT("currInd"), SPF_Mandatory);
		simulationTimeParameter.Bind(Initializer.ParameterMap, TEXT("simulationTime"), SPF_Mandatory);


		TArray_Struct_Parameter_CPU.Bind(Initializer.ParameterMap, TEXT("TArray_FStruct_Parameter_GPU"), SPF_Optional);
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
		Ar 
			<< TArray_Struct_Parameter_CPU 
			<< OutputSurface_Parameter_CPU
			<< numStepsParameter
			<< dTParameter
			<< gridXParameter
			<< gridYParameter
			<< gridZParameter
			<< gridSizeIParameter
			<< gridSizeJParameter
			<< currIndParameter
			<< simulationTimeParameter;
		return bShaderHasOutdatedParams;
	}


	//This function is required to bind our uniform buffers (data) to the shader (by name)
	void BindDataInterfaceToShaderParamName(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef uav)
	{
		if (TArray_Struct_Parameter_CPU.IsBound()) //This should be bound to theTArray_FStruct_Parameter_GPU name in shader
			RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Parameter_CPU.GetBaseIndex(), uav); //bind uav data toTArray_FStruct_Parameter_GPU name
	}

	////This function is required to bind our Constant / Variable buffers (data) to the shader (by name)
	//void BindDataInterfaceToUniformBuffersParamName(FRHICommandList& RHICmdList, FShaderConstants_Class& constants, FVariables_Class& variables)
	//{

	//	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FShaderConstants_Class>(),
	//		FShaderConstants_ClassInstance::CreateUniformBufferImmediate(constants, UniformBuffer_SingleDraw));
	//	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FVariables_Class>(),
	//		FShaderVariables_ClassInstance::CreateUniformBufferImmediate(variables, UniformBuffer_SingleDraw));
	//	/* the uniform FStruct_Shader_GPU_Buffer is temporary, used for a single draw call then discarded
	//	UniformBuffer_SingleDraw = 0
	//	 the uniform FStruct_Shader_GPU_Buffer is used for multiple draw calls but only for the current frame
	//	UniformBuffer_SingleFrame
	//	 the uniform FStruct_Shader_GPU_Buffer is used for multiple draw calls, possibly across multiple frames
	//	UniformBuffer_MultiFrame*/
	//}

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