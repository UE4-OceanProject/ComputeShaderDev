#include "Shader_Interface.h"
#include "WeatherStructs.h"
#include "ShaderPrint.h"

#include "Containers/DynamicRHIResourceArray.h" // Core module

//This links our setup code here, to the actual shader, and entry point (function) we want.
///Declare our shader:   ShaderClassType					ShaderFilePath										    Shader function name		Type
IMPLEMENT_GLOBAL_SHADER(FGlobalComputeShader_Interface, "/Plugin/ComputeShaderDev/Private/WeatherShader.usf", "simulateStep", SF_Compute);


void FGlobalComputeShader_Interface::SetParameters(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	//Render Graph Buildier
	FRDGBuilder GraphBuilder(RHICmdList);

	//Setup your rendering graph here.

	//FParameters needs to be recreated every itteration. Which is why we keep a reference to A_output_ so we don't have to send the buffer data to the GPU every time.
	FParameters* PassParameters;

	//Alloc parameters, this "zeros" out the parameters
	PassParameters = GraphBuilder.AllocParameters<FGlobalComputeShader_Interface::FParameters>();

	//Hardcode our data for now
	int num_input_ = 2; //Number of rows in our array
	TArray<FWarpInConfig2> A_data = {
	{7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7} };


	TResourceArray<FWarpInConfig2> A_output_RA_;
	FRHIResourceCreateInfo A_output_resource_;
	FStructuredBufferRHIRef A_output_buffer_;

	TResourceArray<FWarpInConfig2> B_output_RB_;
	FRHIResourceCreateInfo B_output_resource_;
	FStructuredBufferRHIRef B_output_buffer_;

	TResourceArray<FWarpInConfig2> C_output_RC_;
	FRHIResourceCreateInfo C_output_resource_;
	FStructuredBufferRHIRef C_output_buffer_;



	A_output_RA_.SetNum(num_input_);
	FMemory::Memcpy(A_output_RA_.GetData(), A_data.GetData(), sizeof(FWarpInConfig2) * num_input_);
	A_output_resource_.ResourceArray = &A_output_RA_;
	A_output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, A_output_resource_);
	A_output_UAV_ = RHICreateUnorderedAccessView(A_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	B_output_RB_.SetNum(num_input_);
	FMemory::Memcpy(B_output_RB_.GetData(), A_data.GetData(), sizeof(FWarpInConfig2) * num_input_);
	B_output_resource_.ResourceArray = &B_output_RB_;
	B_output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, B_output_resource_);
	B_output_UAV_ = RHICreateUnorderedAccessView(B_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	C_output_RC_.SetNum(num_input_);
	FMemory::Memcpy(C_output_RC_.GetData(), A_data.GetData(), sizeof(FWarpInConfig2) * num_input_);
	C_output_resource_.ResourceArray = &C_output_RC_;
	C_output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, C_output_resource_);
	C_output_UAV_ = RHICreateUnorderedAccessView(C_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);




	//Set these into an array we will use to rotate these buffers every itteration (past, current, future)
	RotateableBufers = { {A_output_UAV_,B_output_UAV_,C_output_UAV_} };

	//First itteration, don't need to rotate buffers
	PassParameters->test_outputA = A_output_UAV_;
	PassParameters->test_outputB = B_output_UAV_;
	PassParameters->test_outputC = C_output_UAV_;

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
	PassParameters->test_outputA = RotateableBufers[a];
	PassParameters->test_outputB = RotateableBufers[b];
	PassParameters->test_outputC = RotateableBufers[c];

	//Adding a pass
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("Weather compute"),
		this,
		PassParameters,
		FIntVector(1,1,1));  

	GraphBuilder.Execute();
}