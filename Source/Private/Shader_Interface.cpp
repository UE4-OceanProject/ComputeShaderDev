#include "Shader_Interface.h"
#include "WeatherStructs.h"
#include "ShaderPrint.h"

#include "RenderTargetPool.h"

#include "Containers/DynamicRHIResourceArray.h" // Core module


DEFINE_LOG_CATEGORY(WeatherShaderInterface);

//This links our setup code here, to the actual shader, and entry point (function) we want.
///Declare our shader:   ShaderClassType					ShaderFilePath										    Shader function name		Type
IMPLEMENT_GLOBAL_SHADER(FGlobalComputeShader_Interface, "/Plugin/ComputeShaderDev/Private/WeatherShader.usf", "simulateStep", SF_Compute);


void FGlobalComputeShader_Interface::SetParameters(FRHICommandListImmediate& RHICmdList,
	int gridX,
	int gridY,
	int gridZ,
	float gridSizeI,
	float gridSizeJ,
	float dT,
	float simulationTime,
	TArray<float> gridSizeK,
	TArray<FStruct_GroundCellColumns_CPU> ground,
	TArray<FStruct_AirCellColumns_CPU> gridRslow,
	TArray<FStruct_AirCellColumns_CPU> gridInit,
	TArray<FStruct_AirCellColumns_CPU> Grid3D_curr,
	TArray<FStruct_AirCellColumns_CPU> Grid3D_next,
	TArray<FStruct_AirCellColumns_CPU> Grid3D_prev
)
{
	check(IsInRenderingThread());

	int Grid3DSize = gridX * gridY * gridZ;
	int Grid2DSize = gridX * gridY;

	//Render Graph Buildier
//	FRDGBuilder GraphBuilder(RHICmdList);

	//Setup your rendering graph here.

	//FParameters needs to be recreated every itteration. Which is why we keep a reference to Grid3D_curr_ so we don't have to send the buffer data to the GPU every time.
//	FParameters* PassParameters;

	//Alloc parameters, this "zeros" out the parameters
//	PassParameters = GraphBuilder.AllocParameters<FGlobalComputeShader_Interface::FParameters>();


	int num_input_ = gridZ; //Number of rows in our array

	//This is setup this way so it rolls back to 0 on our first run where it should be
	buffer_index_rotator = 2; 

	StepTotalDebug[0] = 0;


	StepTotal_RA_.SetNum(1);
	FMemory::Memcpy(StepTotal_RA_.GetData(), StepTotalDebug.GetData(), sizeof(float) * 1);
	StepTotal_resource_.ResourceArray = &StepTotal_RA_;
	StepTotal_buffer_ = RHICreateStructuredBuffer(sizeof(float), sizeof(float) * 1, BUF_ShaderResource | BUF_UnorderedAccess, StepTotal_resource_);
	StepTotal_UAV_ = RHICreateUnorderedAccessView(StepTotal_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	gridSizeK_RA_.SetNum(gridZ);
	FMemory::Memcpy(gridSizeK_RA_.GetData(), gridSizeK.GetData(), sizeof(float) * gridZ);
	gridSizeK_resource_.ResourceArray = &gridSizeK_RA_;
	gridSizeK_buffer_ = RHICreateStructuredBuffer(sizeof(float), sizeof(float) * gridZ, BUF_ShaderResource | BUF_UnorderedAccess, gridSizeK_resource_);
	gridSizeK_UAV_ = RHICreateUnorderedAccessView(gridSizeK_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);


	ground_RA_.SetNum(Grid2DSize);
	FMemory::Memcpy(ground_RA_.GetData(), ground.GetData(), sizeof(FStruct_AirCellColumns_CPU) * Grid2DSize);
	ground_resource_.ResourceArray = &ground_RA_;
	ground_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * Grid2DSize, BUF_ShaderResource | BUF_UnorderedAccess, ground_resource_);
	ground_UAV_ = RHICreateUnorderedAccessView(ground_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);


	gridRslow_RA_.SetNum(Grid3DSize);
	FMemory::Memcpy(gridRslow_RA_.GetData(), gridRslow.GetData(), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize);
	gridRslow_resource_.ResourceArray = &gridRslow_RA_;
	gridRslow_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize, BUF_ShaderResource | BUF_UnorderedAccess, gridRslow_resource_);
	gridRslow_UAV_ = RHICreateUnorderedAccessView(gridRslow_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	gridInit_RA_.SetNum(Grid3DSize);
	FMemory::Memcpy(gridInit_RA_.GetData(), gridInit.GetData(), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize);
	gridInit_resource_.ResourceArray = &gridInit_RA_;
	gridInit_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize, BUF_ShaderResource | BUF_UnorderedAccess, gridInit_resource_);
	gridInit_UAV_ = RHICreateUnorderedAccessView(gridInit_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);




	Grid3D_curr_RA_.SetNum(Grid3DSize);
	FMemory::Memcpy(Grid3D_curr_RA_.GetData(), Grid3D_curr.GetData(), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize);
	Grid3D_curr_resource_.ResourceArray = &Grid3D_curr_RA_;
	Grid3D_curr_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize, BUF_ShaderResource | BUF_UnorderedAccess, Grid3D_curr_resource_);
	Grid3D_curr_UAV_ = RHICreateUnorderedAccessView(Grid3D_curr_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	Grid3D_next_RB_.SetNum(Grid3DSize);
	FMemory::Memcpy(Grid3D_next_RB_.GetData(), Grid3D_next.GetData(), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize);
	Grid3D_next_resource_.ResourceArray = &Grid3D_next_RB_;
	Grid3D_next_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize, BUF_ShaderResource | BUF_UnorderedAccess, Grid3D_next_resource_);
	Grid3D_next_UAV_ = RHICreateUnorderedAccessView(Grid3D_next_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	Grid3D_prev_RC_.SetNum(Grid3DSize);
	FMemory::Memcpy(Grid3D_prev_RC_.GetData(), Grid3D_prev.GetData(), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize);
	Grid3D_prev_resource_.ResourceArray = &Grid3D_prev_RC_;
	Grid3D_prev_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * Grid3DSize, BUF_ShaderResource | BUF_UnorderedAccess, Grid3D_prev_resource_);
	Grid3D_prev_UAV_ = RHICreateUnorderedAccessView(Grid3D_prev_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);
	
	
	//PassParameters->gridX = gridX;
	//PassParameters->gridY = gridY;
	//PassParameters->gridZ = gridZ;

	//PassParameters->gridSizeI = gridSizeI;
	//PassParameters->gridSizeJ = gridSizeJ;
	//PassParameters->dT = dT;
	//PassParameters->simulationTime = simulationTime;

	////For debugging only
	//PassParameters->StepTotal = StepTotal_UAV_;

	//PassParameters->gridSizeK = gridSizeK_UAV_;
	//PassParameters->ground = ground_UAV_;
	//PassParameters->gridRslow = gridRslow_UAV_;
	//PassParameters->gridInit = gridInit_UAV_;

	////Set these into an array we will use to rotate these buffers every itteration (past, current, future)
	//RotateableBufers = { {Grid3D_curr_UAV_,Grid3D_next_UAV_,Grid3D_prev_UAV_} };

	////First itteration, don't need to rotate buffers
	//PassParameters->Grid3D_curr = Grid3D_curr_UAV_;
	//PassParameters->Grid3D_next = Grid3D_next_UAV_;
	//PassParameters->Grid3D_prev = Grid3D_prev_UAV_;

	////This send everything to the GPU
	//FComputeShaderUtils::AddPass(
	//	GraphBuilder,
	//	RDG_EVENT_NAME("Weather init and compute"),
	//	this,
	//	PassParameters,
	//	FIntVector(1, 1, 1));

	//GraphBuilder.Execute();


	////Lock buffer to enable CPU read
	//char* shaderData = (char*)RHICmdList.LockStructuredBuffer(StepTotal_buffer_, 0, sizeof(float), EResourceLockMode::RLM_ReadOnly);

	////Copy the GPU data back to CPU side (&currentStates)
	////RHICmdList.WriteGPUFence();
	////float* q = (float*)shaderData;
	////float StepTotalDebugA = *q;

	////TArray<float>* p = (TArray<float>*)shaderData;
	////StepTotalDebug = *p;



	//const float* shader_data = (const float*)RHICmdList.LockStructuredBuffer(StepTotal_buffer_, 0, sizeof(float) * 1, EResourceLockMode::RLM_ReadOnly);
	//FMemory::Memcpy(StepTotalDebug.GetData(), shader_data, sizeof(float) * 1);


	//RHICmdList.UnlockStructuredBuffer(StepTotal_buffer_);

	//UE_LOG(WeatherShaderInterface, Display, TEXT("\n //////////////////////////// Step Total Debug  '%f'"), StepTotalDebug[0]);
	////UE_LOG(WeatherShaderInterface, Display, TEXT("\n //////////////////////////// Step Total Debug  '%f'"), StepTotalDebugA);

}

void FGlobalComputeShader_Interface::Compute(FRHICommandListImmediate& RHICmdList,
	int gridX,
	int gridY,
	int gridZ,
	float gridSizeI,
	float gridSizeJ,
	float dT,
	float simulationTime,
	TArray<float> gridSizeK,
	TArray<FStruct_GroundCellColumns_CPU> ground,
	TArray<FStruct_AirCellColumns_CPU> gridRslow,
	TArray<FStruct_AirCellColumns_CPU> gridInit,
	TArray<FStruct_AirCellColumns_CPU> Grid3D_curr,
	TArray<FStruct_AirCellColumns_CPU> Grid3D_next,
	TArray<FStruct_AirCellColumns_CPU> Grid3D_prev,
	UTextureRenderTarget2D* RenderTarget
)
{
	check(IsInRenderingThread());


	FRDGBuilder GraphBuilder(RHICmdList);


	//We need to re-create FParams every itteration
	FParameters* PassParameters;
	//zero out our params
	PassParameters = GraphBuilder.AllocParameters<FGlobalComputeShader_Interface::FParameters>();

	PassParameters->gridX = gridX;
	PassParameters->gridY = gridY;
	PassParameters->gridZ = gridZ;

	PassParameters->gridSizeI = gridSizeI;
	PassParameters->gridSizeJ = gridSizeJ;
	PassParameters->dT = dT;
	PassParameters->simulationTime = simulationTime;

	//For debugging only
	StepTotalDebug[0] = 0;
	StepTotal_RA_.SetNum(1);
	FMemory::Memcpy(StepTotal_RA_.GetData(), StepTotalDebug.GetData(), sizeof(float) * 1);
	StepTotal_resource_.ResourceArray = &StepTotal_RA_;
	StepTotal_buffer_ = RHICreateStructuredBuffer(sizeof(float), sizeof(float) * 1, BUF_ShaderResource | BUF_UnorderedAccess, StepTotal_resource_);
	StepTotal_UAV_ = RHICreateUnorderedAccessView(StepTotal_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);
	PassParameters->StepTotal = StepTotal_UAV_;

	PassParameters->gridSizeK = gridSizeK_UAV_;
	PassParameters->ground = ground_UAV_;
	PassParameters->gridRslow = gridRslow_UAV_;
	PassParameters->gridInit = gridInit_UAV_;














	if (!RenderTarget)
	{
		return;
	}



	FRDGTextureDesc OutputDesc;
	OutputDesc.Extent.X = 512;
	OutputDesc.Extent.Y = 512;
	OutputDesc.Depth = 0;
	OutputDesc.Format = PF_FloatRGBA;
	OutputDesc.NumMips = 1;
	//OutputDesc.Flags = TexCreate_ShaderResource;
	OutputDesc.Flags = TexCreate_None;
	OutputDesc.TargetableFlags = TexCreate_ShaderResource | TexCreate_UAV;
	OutputDesc.bForceSeparateTargetAndShaderResource = false;
	
	           
	FRDGTextureRef HairLUTTexture = GraphBuilder.CreateTexture(OutputDesc, TEXT("HairLUT"));
	PassParameters->OutputTexture = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(HairLUTTexture, 0));












	//Set these into an array we will use to rotate these buffers every itteration (past, current, future)
	RotateableBufers = { {Grid3D_curr_UAV_,Grid3D_next_UAV_,Grid3D_prev_UAV_} };


	//Rotate buffers
	buffer_index_rotator = ((buffer_index_rotator + 1) % 3);
	int buffer_index_rotator2 = ((buffer_index_rotator + 1) % 3);
	int buffer_index_rotator3 = ((buffer_index_rotator2 + 1) % 3);

	int a = buffer_index_rotator;
	int b = buffer_index_rotator2;
	int c = buffer_index_rotator3;


	//Set our rotated references for the simulation
	PassParameters->Grid3D_curr = RotateableBufers[a];
	PassParameters->Grid3D_next = RotateableBufers[b];
	PassParameters->Grid3D_prev = RotateableBufers[c];

	//Adding a pass
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("Weather compute"),
		this,
		PassParameters,
		//This is different than the grouping in shader
		//This will create "Groups of threads"
		//The on in the shader, is the number of thread per group
		FIntVector(16,16,1));  


	GraphBuilder.QueueTextureExtraction(HairLUTTexture, &ComputeShaderOutput, true);

	GraphBuilder.Execute();
	//Lock buffer to enable CPU read
	char* shaderData = (char*)RHICmdList.LockStructuredBuffer(StepTotal_buffer_, 0, sizeof(float), EResourceLockMode::RLM_ReadOnly);
	
	const float* shader_data = (const float*)RHICmdList.LockStructuredBuffer(StepTotal_buffer_, 0, sizeof(float) * 1, EResourceLockMode::RLM_ReadOnly);
	FMemory::Memcpy(StepTotalDebug.GetData(), shader_data, sizeof(float) * 1);

	// Resolve render target
	RHICmdList.CopyToResolveTarget(ComputeShaderOutput.GetReference()->GetRenderTargetItem().TargetableTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FResolveParams());



	RHICmdList.UnlockStructuredBuffer(StepTotal_buffer_);

	UE_LOG(WeatherShaderInterface, Display, TEXT("\n //////////////////////////// Step Total Debug  '%f'"), StepTotalDebug[0]);
}