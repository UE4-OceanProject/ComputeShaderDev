// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "ShaderParameterUtils.h"
#include "WeatherStructs.h"

////Declare the variables inside of our struct
////This FStruct_Shader_CPU_Buffer should contain variables that never, or rarely change
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
////This FStruct_Shader_CPU_Buffer is for variables that change very often (each frame for example)
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
	
	//FShaderResourceParameter TArray_Struct_Parameter_CPU;
	//This is bound to the same data, but exposed to the shader as a UAV
	//FShaderResourceParameter OutputSurface_Parameter_CPU;


	//These members are added to the FShaderParameter class, which will hold information for the runtime to be able
	//to find the bindings, allowing the value of the parameter to be set at runtime.
	//CAN BE UAV! 
	FShaderParameter dTParameter;
	FShaderParameter gridXParameter;
	FShaderParameter gridYParameter;
	FShaderParameter gridZParameter;
	FShaderParameter gridSizeIParameter;
	FShaderParameter gridSizeJParameter;
	FShaderParameter simulationTimeParameter;
	FShaderParameter prevGCParameter;
	FShaderParameter currGCParameter;
	FShaderParameter nextGCParameter;

	FShaderResourceParameter FStruct_Cell_gridSizeK_CPU_ResourceParameter; //single stack of values
	FShaderResourceParameter FStruct_GroundGridContainer_ground_CPU_ResourceParameter; //100
	FShaderResourceParameter FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter; //x5600
	FShaderResourceParameter FStruct_AirGridContainer_gridInit_CPU_ResourceParameter; //x5600

	FShaderResourceParameter FStruct_AirGridContainer_grid3D_CPU_ResourceParameter; //x5600x3


	//Assign a name for each FShaderResourceParamteter for use in the .usf file
	//This happens during shader instance construction
	explicit FGlobalComputeShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{//SPF_Mandatory
	 //Constants
		gridXParameter.Bind(Initializer.ParameterMap, TEXT("gridX"), SPF_Mandatory);
		gridYParameter.Bind(Initializer.ParameterMap, TEXT("gridY"), SPF_Mandatory);
		gridZParameter.Bind(Initializer.ParameterMap, TEXT("gridZ"), SPF_Mandatory);
		gridSizeIParameter.Bind(Initializer.ParameterMap, TEXT("gridSizeI"), SPF_Mandatory);
		gridSizeJParameter.Bind(Initializer.ParameterMap, TEXT("gridSizeJ"), SPF_Mandatory);
		prevGCParameter.Bind(Initializer.ParameterMap, TEXT("prevGC"), SPF_Mandatory);
		currGCParameter.Bind(Initializer.ParameterMap, TEXT("currGC"), SPF_Mandatory);
		nextGCParameter.Bind(Initializer.ParameterMap, TEXT("nextGC"), SPF_Mandatory);

		//Variable
		dTParameter.Bind(Initializer.ParameterMap, TEXT("dT"), SPF_Mandatory);
		simulationTimeParameter.Bind(Initializer.ParameterMap, TEXT("simulationTime"), SPF_Mandatory);

		FStruct_Cell_gridSizeK_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("gridSizeK"), SPF_Mandatory); //single stack of values
		FStruct_GroundGridContainer_ground_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("ground"), SPF_Mandatory); //100
		FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("gridRslow"), SPF_Mandatory); //x5600
		FStruct_AirGridContainer_gridInit_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("gridInit"), SPF_Mandatory); //x5600
		FStruct_AirGridContainer_grid3D_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("grid3D"), SPF_Mandatory); //x5600x3
		//InputSurface.Bind(Initializer.ParameterMap, TEXT("InputSurfaceCS"));
	}

	

	//Example on how to set a simple parameter before shader use
	FStructuredBufferRHIRef SetShaderParameters
	(
		FRHICommandList& RHICmdList,
		float dT,
		int gridX, int gridY, int gridZ, int gridSizeI, int gridSizeJ,
		float simulationTime,
		int prevGC, int currGC, int nextGC,
		TArray<FStruct_Cell_CPU> gridSizeK,
		TArray<FStruct_GroundGridContainer_CPU> ground,
		TArray<FStruct_AirGridContainer_CPU> gridRslow,
		TArray<FStruct_AirGridContainer_CPU> gridInit,
		TArray<FStruct_AirGridContainer_CPU> Grid3D0
	)
	{

		SetShaderValue(RHICmdList, GetComputeShader(), dTParameter, dT);
		SetShaderValue(RHICmdList, GetComputeShader(), gridXParameter, gridX);
		SetShaderValue(RHICmdList, GetComputeShader(), gridYParameter, gridY);
		SetShaderValue(RHICmdList, GetComputeShader(), gridZParameter, gridZ);
		SetShaderValue(RHICmdList, GetComputeShader(), gridSizeIParameter, gridSizeI);
		SetShaderValue(RHICmdList, GetComputeShader(), gridSizeJParameter, gridSizeJ);
		SetShaderValue(RHICmdList, GetComputeShader(), simulationTimeParameter, simulationTime);
		SetShaderValue(RHICmdList, GetComputeShader(), prevGCParameter, prevGC);
		SetShaderValue(RHICmdList, GetComputeShader(), currGCParameter, currGC);
		SetShaderValue(RHICmdList, GetComputeShader(), nextGCParameter, nextGC);


		//A structured buffer is just an array of data consisting of a single data type.
		//You can make a structured buffer of floats, or one of integers, but not one of floats and integers.

		//Global StructuredBuffer that is referenceable by another shader!
		//This is the output texture from the compute shader that we will pass to the pixel shader.
		//This is done when we set the reference (there are various flags that can be sent when setting the reference)
		//BUF_ShaderResource = Shareable between shaders
		//BUF_UnorderedAccess = Allows a UAV interface of this buffer to be created, which allows writting to this buffer

		//FStructuredBufferRHIRef Interface_FStruct_Shader_GPU_Buffer;
		//Since the above parent buffer struct has the BUF_UnorderedAccess flag, we can use this as a writable buffer
		//FUnorderedAccessViewRHIRef Interface_FStruct_Shader_GPU_Buffer_UAV;

		//FStructuredBufferRHIRef Interface_FStruct_AirGridContainer_grid3D_CPU_Data_Buffer;
		//Since the above parent buffer struct has the BUF_UnorderedAccess flag, we can use this as a writable buffer
		//FUnorderedAccessViewRHIRef Interface_FStruct_AirGridContainer_grid3D_CPU_Data_Buffer_UAV;
		//FUnorderedAccessViewRHIRef Interface_FStruct_AirGridContainer_grid3D_CPU_Data_Buffer_UAV;
		//FUnorderedAccessViewRHIRef Interface_FStruct_AirGridContainer_grid3D_CPU_Data_Buffer_UAV;
		//FUnorderedAccessViewRHIRef Interface_FStruct_AirGridContainer_grid3D_CPU_Data_Buffer_UAV;
		//FUnorderedAccessViewRHIRef Interface_FStruct_AirGridContainer_grid3D_CPU_Data_Buffer_UAV;

		//You can do the same thing with a regular texture
		//FTexture2DRHIRef Texture;
		//FUnorderedAccessViewRHIRef TextureUAV;







		////Create TResourceArray class
		////If we set bNeedsCPUAccess, we could remove the middleman (currentStates) and call Discard() ourselves ?
		////when we don't need it anymore (does this have to be render thread data?
		//TResourceArray<FStruct_AirGridContainer_CPU> FStruct_AirGridContainer_CPU_Data;

		TResourceArray<FStruct_Cell_CPU> FStruct_Cell_gridSizeK_CPU_Data; //single stack of values
		TResourceArray<FStruct_GroundGridContainer_CPU>FStruct_GroundGridContainer_ground_CPU_Data; //100
		TResourceArray<FStruct_AirGridContainer_CPU>FStruct_AirGridContainer_gridRslow_CPU_Data; //x5600
		TResourceArray<FStruct_AirGridContainer_CPU>FStruct_AirGridContainer_gridInit_CPU_Data; //x5600
		TResourceArray<FStruct_AirGridContainer_CPU>FStruct_AirGridContainer_grid3D_CPU_Data; //x5600x3


		FStruct_Cell_gridSizeK_CPU_Data.Append(gridSizeK); //single stack of values
		FStruct_GroundGridContainer_ground_CPU_Data.Append(ground); //100
		FStruct_AirGridContainer_gridRslow_CPU_Data.Append(gridRslow); //x5600
		FStruct_AirGridContainer_gridInit_CPU_Data.Append(gridInit); //x5600
		FStruct_AirGridContainer_grid3D_CPU_Data.Append(Grid3D0); //x5600x3
//		FStruct_AirGridContainer_grid3D_CPU_Data.Append(Grid3D0[1]); //x5600x3
//		FStruct_AirGridContainer_grid3D_CPU_Data.Append(Grid3D0[2]); //x5600x3

		////FStruct_AirGridContainer_CPU_Data.SetAllowCPUAccess(true);
		/**
		* @return A pointer to the resource data.
		*/
		//FStruct_Cell_gridSizeK_CPU_Data.GetResourceData();

		/**
		* @return size of resource data allocation
		*/
		//FStruct_Cell_gridSizeK_CPU_Data.GetResourceDataSize();

		//struct of information for a future resource on the CPU
		//FRHIResourceCreateInfo FCPU_Resource_Info;
		FRHIResourceCreateInfo FStruct_Cell_gridSizeK_CPU_Data_Resource_Info(&FStruct_Cell_gridSizeK_CPU_Data);
		FRHIResourceCreateInfo FStruct_GroundGridContainer_ground_CPU_Data_Resource_Info(&FStruct_GroundGridContainer_ground_CPU_Data);
		FRHIResourceCreateInfo FStruct_AirGridContainer_gridRslow_CPU_Data_Resource_Info(&FStruct_AirGridContainer_gridRslow_CPU_Data);
		FRHIResourceCreateInfo FStruct_AirGridContainer_gridInit_CPU_Data_Resource_Info(&FStruct_AirGridContainer_gridInit_CPU_Data);
		FRHIResourceCreateInfo FStruct_AirGridContainer_grid3D_CPU_Data_Resource_Info(&FStruct_AirGridContainer_grid3D_CPU_Data);

		//}
		////AND RIGHT HERE JUST PASS THE REFERENCE TO OUR ARRAY!
		////Now put a reference to this data into our FCPU_Resource_Info class

		//FCPU_Resource_Info.ResourceArray = &FStruct_AirGridContainer_CPU_Data;
		//FStruct_Cell_gridSizeK_CPU_Data_Resource_Info.ResourceArray = &FStruct_Cell_gridSizeK_CPU_Data;
		//FStruct_GroundGridContainer_ground_CPU_Data_Resource_Info.ResourceArray = &FStruct_GroundGridContainer_ground_CPU_Data;
		//FStruct_AirGridContainer_gridRslow_CPU_Data_Resource_Info.ResourceArray = &FStruct_AirGridContainer_gridRslow_CPU_Data;
		//FStruct_AirGridContainer_gridInit_CPU_Data_Resource_Info.ResourceArray = &FStruct_AirGridContainer_gridInit_CPU_Data;
		//FStruct_AirGridContainer_grid3D_CPU_Data_Resource_Info.ResourceArray = &FStruct_AirGridContainer_grid3D_CPU_Data;


		////FResourceBulkDataInterface <- Allows for direct CPU mem allocation for bulk resource types.



		////Initializing the buffer and writing data to CPU
		////Create various interfaces for our TResourceArray Data (FCPU_Resource_Info)
		//Interface_FStruct_Shader_CPU_Buffer = RHICreateStructuredBuffer(sizeof(FStruct_AirGridContainer_CPU), sizeof(FStruct_AirGridContainer_CPU) * 10, BUF_UnorderedAccess | BUF_ShaderResource | 0, FCPU_Resource_Info);
		//UAV flag so we can read back to cpu
		FStructuredBufferRHIRef FStruct_Cell_gridSizeK_CPU_Data_BufferInterfaceSRV = 
			RHICreateStructuredBuffer(sizeof(FStruct_Cell_CPU),
				FStruct_Cell_gridSizeK_CPU_Data.Num() * sizeof(FStruct_Cell_CPU),
			BUF_UnorderedAccess | BUF_ShaderResource | 0, FStruct_Cell_gridSizeK_CPU_Data_Resource_Info
			);

		FStructuredBufferRHIRef FStruct_GroundGridContainer_ground_CPU_Data_BufferInterfaceSRV = 
		RHICreateStructuredBuffer(sizeof(FStruct_GroundGridContainer_CPU),
			FStruct_GroundGridContainer_ground_CPU_Data.Num() * sizeof(FStruct_GroundGridContainer_CPU),
			BUF_UnorderedAccess | BUF_ShaderResource | 0, FStruct_GroundGridContainer_ground_CPU_Data_Resource_Info);

		FStructuredBufferRHIRef FStruct_AirGridContainer_gridRslow_CPU_Data_BufferInterfaceSRV = 
		RHICreateStructuredBuffer(sizeof(FStruct_AirGridContainer_CPU),
			FStruct_AirGridContainer_gridRslow_CPU_Data.Num() * sizeof(FStruct_AirGridContainer_CPU),
			BUF_UnorderedAccess | BUF_ShaderResource | 0, FStruct_AirGridContainer_gridRslow_CPU_Data_Resource_Info);

		FStructuredBufferRHIRef FStruct_AirGridContainer_gridInit_CPU_Data_BufferInterfaceSRV = 
		RHICreateStructuredBuffer(
			sizeof(FStruct_AirGridContainer_CPU),
			FStruct_AirGridContainer_gridInit_CPU_Data.Num() * sizeof(FStruct_AirGridContainer_CPU),
			BUF_UnorderedAccess | BUF_ShaderResource | 0, 
			FStruct_AirGridContainer_gridInit_CPU_Data_Resource_Info);


		int stride = sizeof(FStruct_AirCellColumns_CPU);
		int size = FStruct_AirGridContainer_grid3D_CPU_Data[0].CellColumns.Num()* 3 * sizeof(FStruct_AirCellColumns_CPU);
		int ActualSize = FStruct_AirGridContainer_grid3D_CPU_Data_Resource_Info.ResourceArray->GetResourceDataSize();

		FStructuredBufferRHIRef FStruct_AirGridContainer_grid3D_CPU_Data_BufferInterfaceSRV = 
		RHICreateStructuredBuffer(
			sizeof(FStruct_AirGridContainer_CPU),
			FStruct_AirGridContainer_grid3D_CPU_Data.Num() * sizeof(FStruct_AirGridContainer_CPU),
			BUF_UnorderedAccess | BUF_ShaderResource | 0, 
			FStruct_AirGridContainer_grid3D_CPU_Data_Resource_Info);

		////--------------------------------------------------------
		////At this point our TResourceArray (FStruct_AirGridContainer_CPU_Data) will destroy itself once its finished copying its data to CPU!
		////Unless bNeedsCPUAccess is set to true!
		//Interface_FStruct_Shader_CPU_Buffer_UAV = RHICreateUnorderedAccessView(Interface_FStruct_Shader_CPU_Buffer, false, false);
		
		FUnorderedAccessViewRHIRef FStruct_Cell_gridSizeK_CPU_Data_BufferInterfaceSRV_UAV = RHICreateUnorderedAccessView(FStruct_Cell_gridSizeK_CPU_Data_BufferInterfaceSRV, false, false);
		FUnorderedAccessViewRHIRef FStruct_GroundGridContainer_ground_CPU_Data_BufferInterfaceSRV_UAV = RHICreateUnorderedAccessView(FStruct_GroundGridContainer_ground_CPU_Data_BufferInterfaceSRV, false, false);
		FUnorderedAccessViewRHIRef FStruct_AirGridContainer_gridRslow_CPU_Data_BufferInterfaceSRV_UAV = RHICreateUnorderedAccessView(FStruct_AirGridContainer_gridRslow_CPU_Data_BufferInterfaceSRV, false, false);
		FUnorderedAccessViewRHIRef FStruct_AirGridContainer_gridInit_CPU_Data_BufferInterfaceSRV_UAV = RHICreateUnorderedAccessView(FStruct_AirGridContainer_gridInit_CPU_Data_BufferInterfaceSRV, false, false);
		FUnorderedAccessViewRHIRef FStruct_AirGridContainer_grid3D_CPU_Data_BufferInterfaceSRV_UAV = RHICreateUnorderedAccessView(FStruct_AirGridContainer_grid3D_CPU_Data_BufferInterfaceSRV, false, false);



		SetUAVParameter(RHICmdList, GetComputeShader(), FStruct_Cell_gridSizeK_CPU_ResourceParameter, FStruct_Cell_gridSizeK_CPU_Data_BufferInterfaceSRV_UAV); //single stack of values
		SetUAVParameter(RHICmdList, GetComputeShader(), FStruct_GroundGridContainer_ground_CPU_ResourceParameter, FStruct_GroundGridContainer_ground_CPU_Data_BufferInterfaceSRV_UAV); //100
		SetUAVParameter(RHICmdList, GetComputeShader(), FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter, FStruct_AirGridContainer_gridRslow_CPU_Data_BufferInterfaceSRV_UAV); //x5600
		SetUAVParameter(RHICmdList, GetComputeShader(), FStruct_AirGridContainer_gridInit_CPU_ResourceParameter, FStruct_AirGridContainer_gridInit_CPU_Data_BufferInterfaceSRV_UAV); //x5600
		
		SetUAVParameter(RHICmdList, GetComputeShader(), FStruct_AirGridContainer_grid3D_CPU_ResourceParameter, FStruct_AirGridContainer_grid3D_CPU_Data_BufferInterfaceSRV_UAV); //x5600x3
		
		return FStruct_AirGridContainer_grid3D_CPU_Data_BufferInterfaceSRV;
	}

	


	// FShader interface: tells ParameterMap which FShaderResourceParameters we want to bind
	//Updates cached ParameterMap if we add a new param here
	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);
		//Ar << YourResourceName1 << YourResourceName2 << YourResourceName3 << ......;
		Ar
			//			<< TArray_Struct_Parameter_CPU 
			//			<< OutputSurface_Parameter_CPU
			<< dTParameter
			<< gridXParameter
			<< gridYParameter
			<< gridZParameter
			<< gridSizeIParameter
			<< gridSizeJParameter
			<< simulationTimeParameter
			<< prevGCParameter
			<< currGCParameter
			<< nextGCParameter

			<< FStruct_Cell_gridSizeK_CPU_ResourceParameter; //single stack of values
			//<< FStruct_GroundGridContainer_ground_CPU_ResourceParameter //100
			//<< FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter //x5600
			//<< FStruct_AirGridContainer_gridInit_CPU_ResourceParameter //x5600

			//<< FStruct_AirGridContainer_grid3D_CPU_ResourceParameter; //x5600x3

		return bShaderHasOutdatedParams;
	}


	//This function is required to bind our uniform buffers (data) to the shader (by name)
//	void BindDataInterfaceToShaderParamName(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef uav)
//	{
//		if (TArray_Struct_Parameter_CPU.IsBound()) //This should be bound to theTArray_FStruct_Parameter_CPU name in shader
//			RHICmdList.SetUAVParameter(GetComputeShader(), TArray_Struct_Parameter_CPU.GetBaseIndex(), uav); //bind uav data toTArray_FStruct_Parameter_CPU name
//	}

	////This function is required to bind our Constant / Variable buffers (data) to the shader (by name)
	//void BindDataInterfaceToUniformBuffersParamName(FRHICommandList& RHICmdList, FShaderConstants_Class& constants, FVariables_Class& variables)
	//{

	//	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FShaderConstants_Class>(),
	//		FShaderConstants_ClassInstance::CreateUniformBufferImmediate(constants, UniformBuffer_SingleDraw));
	//	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FVariables_Class>(),
	//		FShaderVariables_ClassInstance::CreateUniformBufferImmediate(variables, UniformBuffer_SingleDraw));
	//	/* the uniform FStruct_Shader_CPU_Buffer is temporary, used for a single draw call then discarded
	//	UniformBuffer_SingleDraw = 0
	//	 the uniform FStruct_Shader_CPU_Buffer is used for multiple draw calls but only for the current frame
	//	UniformBuffer_SingleFrame
	//	 the uniform FStruct_Shader_CPU_Buffer is used for multiple draw calls, possibly across multiple frames
	//	UniformBuffer_MultiFrame*/
	//}

	//This is used to clean up the FStruct_Shader_CPU_Buffer binds after each invocation to let them be changed and used elsewhere if needed.
	void UnbindBuffers(FRHICommandList& RHICmdList)
	{
		if (FStruct_Cell_gridSizeK_CPU_ResourceParameter.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), FStruct_Cell_gridSizeK_CPU_ResourceParameter.GetBaseIndex(), FUnorderedAccessViewRHIRef());
		if (FStruct_GroundGridContainer_ground_CPU_ResourceParameter.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), FStruct_GroundGridContainer_ground_CPU_ResourceParameter.GetBaseIndex(), FUnorderedAccessViewRHIRef());
		if (FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter.GetBaseIndex(), FUnorderedAccessViewRHIRef());
		if (FStruct_AirGridContainer_gridInit_CPU_ResourceParameter.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), FStruct_AirGridContainer_gridInit_CPU_ResourceParameter.GetBaseIndex(), FUnorderedAccessViewRHIRef());
		if (FStruct_AirGridContainer_grid3D_CPU_ResourceParameter.IsBound())
		RHICmdList.SetUAVParameter(GetComputeShader(), FStruct_AirGridContainer_grid3D_CPU_ResourceParameter.GetBaseIndex(), FUnorderedAccessViewRHIRef());
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
		//if (Platform != PLATFORM_WINDOWS)
		//	return false;
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	}

	//Here you can modify compiliation flags and also modify Defines in the usf file before compiliation
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
		OutEnvironment.CompilerFlags.Add(CFLAG_OnChip);
		OutEnvironment.CompilerFlags.Add(CFLAG_PreferFlowControl);
		// Add your own defines for the shader code
		//OutEnvironment.SetDefine(TEXT("MY_DEFINE"), 1);
		//OutEnvironment.SetDefine(TEXT("NAME"), TEXT("Test"));

	}

};