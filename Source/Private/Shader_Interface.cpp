#include "Shader_Interface.h"
#include "WeatherStructs.h"
#include "ShaderPrint.h"

///Declare our shader:   ShaderClassType					ShaderFilePath										    Shader function name		Type
IMPLEMENT_GLOBAL_SHADER(FGlobalComputeShader_Interface, "/Plugin/ComputeShaderDev/Private/WeatherShader.usf", "simulateStep", SF_Compute);

/** Descriptor of a graph tracked texture. */
using FRDGTextureDesc = FPooledRenderTargetDesc;

void FGlobalComputeShader_Interface::SetParameters(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());



	//Render Graph Buildier
	FRDGBuilder GraphBuilder(RHICmdList);
	//Setup your rendering graph here.


	//Creating a texture
	FRDGTextureDesc SceneColorDesc = FRDGTextureDesc::Create2DDesc(
		FIntPoint(1920, 1080),
		PF_FloatRGBA,
		FClearValueBinding::Black,
		/* InFlags = */ TexCreate_None,
		/* InTargetableFlags = */ TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_UAV,
		/* bInForceSeparateTargetAndShaderResource = */ false);
	FRDGTexture* SceneColor = GraphBuilder.CreateTexture(SceneColorDesc, TEXT("SceneColor"));

	//Creating an SRV for a Texture
	FRDGTextureSRVDesc Desc = FRDGTextureSRVDesc::Create(SceneColor);
	FRDGTextureSRVRef MipSRV = GraphBuilder.CreateSRV(Desc);

	//Creating a UAV for a Texture
	FRDGTextureUAV* SceneColorUAV = GraphBuilder.CreateUAV(SceneColor);

	FParameters* PassParameters;

	//Alloc parameters
	//PassParameters = GraphBuilder.AllocateForRHILifeTime<FGlobalComputeShader_Interface::FParameters>();
	PassParameters = GraphBuilder.AllocParameters<FGlobalComputeShader_Interface::FParameters>();








	//Setup parameters
	//PassParameters->ShaderPrintUniformBuffer.RWValuesBuffer = SceneColorUAV->GetRHI();
//ClearUnusedGraphResources()


	int num_input_ = 2;



	TArray<FWarpInConfig2> A_data = {
{7,7,7,7,7,7,7,7,7,7},
{7,7,7,7,7,7,7,7,7,7} };

	A_output_RA_.SetNum(num_input_);
	FMemory::Memcpy(A_output_RA_.GetData(), A_data.GetData(), sizeof(FWarpInConfig2) * num_input_);
	A_output_resource_.ResourceArray = &A_output_RA_;
	A_output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, A_output_resource_);
	A_output_UAV_ = RHICreateUnorderedAccessView(A_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	PassParameters->test_outputA = A_output_UAV_;









	//Adding a pass
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("MyShader %dx%d", 1920, 1080),
		this,
		PassParameters,
		FIntVector(1, 1, 1));




	GraphBuilder.Execute();

}

void FGlobalComputeShader_Interface::Compute(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());



	//Render Graph Buildier
	FRDGBuilder GraphBuilder(RHICmdList);
	//Setup your rendering graph here.


	FParameters* PassParameters;

	//Alloc parameters
	//PassParameters = GraphBuilder.AllocateForRHILifeTime<FGlobalComputeShader_Interface::FParameters>();
	PassParameters = GraphBuilder.AllocParameters<FGlobalComputeShader_Interface::FParameters>();
	A_output_UAV_ = RHICreateUnorderedAccessView(A_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	PassParameters->test_outputA = A_output_UAV_;

	//Adding a pass
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("MyShader %dx%d", 1920, 1080),
		this,
		PassParameters,
		FIntVector(1,1,1));  




	GraphBuilder.Execute();

}



//void FGlobalComputeShader_Interface::Setup_RenderThread()
//{
//	check(IsInRenderingThread());
//
//
//
//
//	// In this sample code, output_buffer_ has not input values, so what we need here is just the pointer to output_resource_.
//	A_output_RA_.SetNum(num_input_);
//	A_output_RA_.Init({ 7,7,7,7,7,7,7,7,7,7 }, 2);
//
//	A_output_resource_.ResourceArray = &A_output_RA_;
//	A_output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, A_output_resource_);
//	A_output_UAV_ = RHICreateUnorderedAccessView(A_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);
//
//
//	//FGlobalComputeShader_Interface::
//
//
//	//FGlobalWeatherParameters GlobalWeatherParams; //Change this back when fix is in place
//
//
//	//GlobalWeatherParams.test_outputA = GlobalWeatherParams.test_outputA = A_output_UAV_; //Change this back when fix is in place
//
//	//Must be done on render thread
//	//MainWeatherParams.GlobalWeatherParameters = FUniformBufferRef::CreateUniformBufferImmediate(GlobalWeatherParams, UniformBuffer_MultiFrame); //Change this back when fix is in place
//	//or try this (guys said above code is bad, because reference will dissapear when scope exit
//	//MainWeatherParams.test_outputA = GlobalWeatherParams.test_outputA = A_output_UAV_;; //Change this back when fix is in place
//
//
//	//MainWeatherParams.test_outputA = A_output_UAV_; //Change this back when fix is in place //Change this back when fix is in place
//	ShaderPrint_Custom::SetParameters(MainWeatherParams.ShaderPrintUniformBuffer);
//
//	
//	
//	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
//
//	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);
//
//	// Get global RHI command list
//	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
//
//	RHICmdList.SetComputeShader(ComputeShader->GetComputeShader());//Change this back when fix is in place??
//
//	TArray<FShaderResourceParameter>RotateableBufers = { {A_output_} };//Change this back when fix is in place??
//	RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[0].GetBaseIndex(), A_output_UAV_);//Change this back when fix is in place??
//
//	DispatchComputeShader(RHICmdList, *ComputeShader, 1, 1, 1); 
//	//FComputeShaderUtils::Dispatch(RHICmdList, *ComputeShader, MainWeatherParams, FIntVector(1, 1, 1));//Change this back when fix is in place
//
//	ShaderPrint_Custom::BeginView(RHICmdList);
//}
//
//
//
//void FGlobalComputeShader_Interface::Calculate_RenderThread()
//{
//	check(IsInRenderingThread());
//
//	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
//
//	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);
//
//	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
//	RHICmdList.SetComputeShader(ComputeShader->GetComputeShader());
//	TArray<FShaderResourceParameter>RotateableBufers = { {A_output_} };//Still needed or we don't get a buffer in our shader // even though we are not setting the data. 
//	RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[0].GetBaseIndex(), A_output_UAV_);//Still needed or we don't get a buffer in our shader // even though we are not setting the data. 
//	DispatchComputeShader(RHICmdList, *ComputeShader, 1, 1, 1);
//
//	FRDGBuilder GraphBuilder(RHICmdList);
//
//	//const FSceneTextureParameters* SceneTextures = nullptr;
//
//	//FRDGTextureRef Texture = MainWeatherParams.ShaderPrintUniformBuffer.RWValuesBuffer;
//	////FRDGTextureRef Texture = //MainWeatherParams.ShaderPrintUniformBuffer
//	////FRDGTextureRef Texture = UAV->Desc.Texture;
//	//FPostProcessingInputs PostProcessingInputs;
//
//		// View.ViewRect.{Width,Height}() are most likely to be < 2^24, so the float
//	// conversion won't loss any precision (assuming float have 23bits for mantissa)
//	const int32 NumMipsX = FMath::Max(FPlatformMath::CeilToInt(FMath::Log2(float(1024))) - 1, 1);
//	const int32 NumMipsY = FMath::Max(FPlatformMath::CeilToInt(FMath::Log2(float(800))) - 1, 1);
//	const uint32 NumMips = FMath::Max(NumMipsX, NumMipsY);
//
//	// Must be power of 2
//	const FIntPoint HZBSize(1 << NumMipsX, 1 << NumMipsY);
//
//
//	//@DW: Configure texture creation
//	FRDGTextureDesc HZBDesc = FRDGTextureDesc::Create2DDesc(HZBSize, PF_R16F, FClearValueBinding::None, TexCreate_None, TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_NoFastClear, false, NumMips);
//	HZBDesc.Flags;
//
//	//@DW: Explicit creation of graph resource handles - full support for everything the RHI supports
//	//@DW: Now that we've created a resource handle, it will have to be passed around to other passes for manual wiring or put into a Blackboard structure for automatic wiring
//	FRDGTextureRef HZBTexture = GraphBuilder.CreateTexture(HZBDesc, TEXT("HZB"));
//	ShaderPrint_Custom::DrawView(GraphBuilder, HZBTexture);
//}