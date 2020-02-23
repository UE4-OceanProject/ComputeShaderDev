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



	TResourceArray<FWarpInConfig2> A_output_RA_;
	FRHIResourceCreateInfo A_output_resource_;
	FStructuredBufferRHIRef A_output_buffer_;

	int num_input_ = 2; //Number of rows in our array
	TArray<FWarpInConfig2> A_data = {
	{7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7} };

	A_output_RA_.SetNum(num_input_);
	FMemory::Memcpy(A_output_RA_.GetData(), A_data.GetData(), sizeof(FWarpInConfig2) * num_input_);
	A_output_resource_.ResourceArray = &A_output_RA_;
	A_output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, A_output_resource_);
	A_output_UAV_ = RHICreateUnorderedAccessView(A_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);

	PassParameters->test_outputA = A_output_UAV_;

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

	FParameters* PassParameters;
	PassParameters = GraphBuilder.AllocParameters<FGlobalComputeShader_Interface::FParameters>();

	PassParameters->test_outputA = A_output_UAV_;

	//Adding a pass
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("Weather compute"),
		this,
		PassParameters,
		FIntVector(1,1,1));  

	GraphBuilder.Execute();
}