#include "Shader_Interface.h"
#include "WeatherStructs.h"
#include "ShaderPrint.h"

#include "Containers/DynamicRHIResourceArray.h" // Core module

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

	//Render Graph Buildier
	FRDGBuilder GraphBuilder(RHICmdList);

	//Setup your rendering graph here.

	//FParameters needs to be recreated every itteration. Which is why we keep a reference to Grid3D_curr_ so we don't have to send the buffer data to the GPU every time.
	FParameters* PassParameters;

	//Alloc parameters, this "zeros" out the parameters
	PassParameters = GraphBuilder.AllocParameters<FGlobalComputeShader_Interface::FParameters>();

	//Hardcode our data for now
	int num_input_ = gridZ; //Number of rows in our array


		TResourceArray<FStruct_AirCellColumns_CPU> gridSizeK_RA_;
	FRHIResourceCreateInfo gridSizeK_resource_;
	FStructuredBufferRHIRef gridSizeK_buffer_;


		TResourceArray<FStruct_AirCellColumns_CPU> ground_RA_;
	FRHIResourceCreateInfo ground_resource_;
	FStructuredBufferRHIRef ground_buffer_;

		TResourceArray<FStruct_AirCellColumns_CPU> gridRslow_RA_;
	FRHIResourceCreateInfo gridRslow_resource_;
	FStructuredBufferRHIRef gridRslow_buffer_;

		TResourceArray<FStruct_AirCellColumns_CPU> gridInit_RA_;
	FRHIResourceCreateInfo gridInit_resource_;
	FStructuredBufferRHIRef gridInit_buffer_;


	TResourceArray<FStruct_AirCellColumns_CPU> Grid3D_curr_RA_;
	FRHIResourceCreateInfo Grid3D_curr_resource_;
	FStructuredBufferRHIRef Grid3D_curr_buffer_;

	TResourceArray<FStruct_AirCellColumns_CPU> Grid3D_next_RB_;
	FRHIResourceCreateInfo Grid3D_next_resource_;
	FStructuredBufferRHIRef Grid3D_next_buffer_;

	TResourceArray<FStruct_AirCellColumns_CPU> Grid3D_prev_RC_;
	FRHIResourceCreateInfo Grid3D_prev_resource_;
	FStructuredBufferRHIRef Grid3D_prev_buffer_;



	gridSizeK_RA_.SetNum(num_input_);
	FMemory::Memcpy(gridSizeK_RA_.GetData(), gridSizeK.GetData(), sizeof(FStruct_AirCellColumns_CPU) * num_input_);
	gridSizeK_resource_.ResourceArray = &gridSizeK_RA_;
	gridSizeK_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, gridSizeK_resource_);
	gridSizeK_UAV_ = RHICreateUnorderedAccessView(gridSizeK_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);


	ground_RA_.SetNum(num_input_);
	FMemory::Memcpy(ground_RA_.GetData(), ground.GetData(), sizeof(FStruct_AirCellColumns_CPU) * num_input_);
	ground_resource_.ResourceArray = &ground_RA_;
	ground_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, ground_resource_);
	ground_UAV_ = RHICreateUnorderedAccessView(ground_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);


	gridRslow_RA_.SetNum(num_input_);
	FMemory::Memcpy(gridRslow_RA_.GetData(), gridRslow.GetData(), sizeof(FStruct_AirCellColumns_CPU) * num_input_);
	gridRslow_resource_.ResourceArray = &gridRslow_RA_;
	gridRslow_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, gridRslow_resource_);
	gridRslow_UAV_ = RHICreateUnorderedAccessView(gridRslow_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	gridInit_RA_.SetNum(num_input_);
	FMemory::Memcpy(gridInit_RA_.GetData(), gridInit.GetData(), sizeof(FStruct_AirCellColumns_CPU) * num_input_);
	gridInit_resource_.ResourceArray = &gridInit_RA_;
	gridInit_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, gridInit_resource_);
	gridInit_UAV_ = RHICreateUnorderedAccessView(gridInit_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);




	Grid3D_curr_RA_.SetNum(num_input_);
	FMemory::Memcpy(Grid3D_curr_RA_.GetData(), Grid3D_curr.GetData(), sizeof(FStruct_AirCellColumns_CPU) * num_input_);
	Grid3D_curr_resource_.ResourceArray = &Grid3D_curr_RA_;
	Grid3D_curr_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, Grid3D_curr_resource_);
	Grid3D_curr_UAV_ = RHICreateUnorderedAccessView(Grid3D_curr_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	Grid3D_next_RB_.SetNum(num_input_);
	FMemory::Memcpy(Grid3D_next_RB_.GetData(), Grid3D_next.GetData(), sizeof(FStruct_AirCellColumns_CPU) * num_input_);
	Grid3D_next_resource_.ResourceArray = &Grid3D_next_RB_;
	Grid3D_next_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, Grid3D_next_resource_);
	Grid3D_next_UAV_ = RHICreateUnorderedAccessView(Grid3D_next_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	Grid3D_prev_RC_.SetNum(num_input_);
	FMemory::Memcpy(Grid3D_prev_RC_.GetData(), Grid3D_prev.GetData(), sizeof(FStruct_AirCellColumns_CPU) * num_input_);
	Grid3D_prev_resource_.ResourceArray = &Grid3D_prev_RC_;
	Grid3D_prev_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirCellColumns_CPU), sizeof(FStruct_AirCellColumns_CPU) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, Grid3D_prev_resource_);
	Grid3D_prev_UAV_ = RHICreateUnorderedAccessView(Grid3D_prev_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);
	
	
	PassParameters->gridX = gridX;
	PassParameters->gridY = gridY;
	PassParameters->gridZ = gridZ;

	PassParameters->gridSizeI = gridSizeI;
	PassParameters->gridSizeJ = gridSizeJ;
	PassParameters->dT = dT;
	PassParameters->simulationTime = simulationTime;

	PassParameters->gridSizeK = gridSizeK_UAV_;
	PassParameters->ground = ground_UAV_;
	PassParameters->gridRslow = gridRslow_UAV_;
	PassParameters->gridInit = gridInit_UAV_;

	//Set these into an array we will use to rotate these buffers every itteration (past, current, future)
	RotateableBufers = { {Grid3D_curr_UAV_,Grid3D_next_UAV_,Grid3D_prev_UAV_} };

	//First itteration, don't need to rotate buffers
	PassParameters->Grid3D_curr = Grid3D_curr_UAV_;
	PassParameters->Grid3D_next = Grid3D_next_UAV_;
	PassParameters->Grid3D_prev = Grid3D_prev_UAV_;

	//This send everything to the GPU
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("Weather init and compute"),
		this,
		PassParameters,
		FIntVector(1, 1, 1));

	GraphBuilder.Execute();

}

void FGlobalComputeShader_Interface::Compute(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());


	FRDGBuilder GraphBuilder(RHICmdList);


	//We need to re-create FParams every itteration
	FParameters* PassParameters;
	//zero out our params
	PassParameters = GraphBuilder.AllocParameters<FGlobalComputeShader_Interface::FParameters>();


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
		FIntVector(1,1,1));  

	GraphBuilder.Execute();
}