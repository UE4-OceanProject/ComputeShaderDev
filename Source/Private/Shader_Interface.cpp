#include "Shader_Interface.h"
#include "ShaderParameterUtils.h"
#include "Shader_Interface_Parameters.h"
#include "WeatherStructs.h"
#include "CoreMinimal.h"
#include "GlobalShader.h" //ShaderCore module
#include "UniformBuffer.h" // RenderCore module
#include "RHICommandList.h" // RHI module

#include "Shader.h"
#include "GlobalShader.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "RenderCommandFence.h"

#include "ShaderPrintParameters.h"
#include "RenderGraphUtils.h"
#include "RenderGraphResources.h"

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FGlobalWeatherParameters, "WeatherData");
typedef TUniformBufferRef<FGlobalWeatherParameters> FUniformBufferRef;


void FGlobalComputeShader_Interface::SetParameters()
{

	ENQUEUE_RENDER_COMMAND(SetupCommand)//TypeName - Arbitrary name of the render command
		(
			[this](FRHICommandListImmediate& RHICmdList) //Passed in variables
	{
		//This code block is ran inside of the Render Thread!
		//Which is why we need the reference to our FrontEnd class
		this->Setup_RenderThread();
	}
			//This is now back in the Game Thread!
	);


	render_command_fence_.BeginFence();
	render_command_fence_.Wait(); // Waits for pending fence commands to retire.

	UE_LOG(LogTemp, Warning, TEXT("===== Calculate ====="));

}

void FGlobalComputeShader_Interface::Setup_RenderThread()
{
	check(IsInRenderingThread());

	TArray<FWarpInConfig2> A_data = {
	{7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7} };


	TResourceArray<FWarpInConfig2> A_output_RA_; // Not necessary.
	FRHIResourceCreateInfo A_output_resource_;
	FStructuredBufferRHIRef A_output_buffer_;
	FUnorderedAccessViewRHIRef A_output_UAV_;


	// In this sample code, output_buffer_ has not input values, so what we need here is just the pointer to output_resource_.
	A_output_RA_.SetNum(num_input_);
	FMemory::Memcpy(A_output_RA_.GetData(), A_data.GetData(), sizeof(FWarpInConfig2) * num_input_);
	A_output_resource_.ResourceArray = &A_output_RA_;
	A_output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, A_output_resource_);
	A_output_UAV_ = RHICreateUnorderedAccessView(A_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);


	//FGlobalComputeShader_Interface::
	FGlobalComputeShader_Interface::FParameters MainWeatherParams;


	FGlobalWeatherParameters GlobalWeatherParams;


	GlobalWeatherParams.test_outputA = GlobalWeatherParams.test_outputA = A_output_UAV_;

	//Must be done on render thread
	MainWeatherParams.GlobalWeatherParameters = FUniformBufferRef::CreateUniformBufferImmediate(GlobalWeatherParams, UniformBuffer_MultiFrame);


	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);

	// Get global RHI command list
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();


	FComputeShaderUtils::Dispatch(RHICmdList, *ComputeShader, MainWeatherParams, FIntVector(1, 1, 1));

}

void FGlobalComputeShader_Interface::Compute() 
{
	ENQUEUE_RENDER_COMMAND(ComputeCommand)//TypeName - Arbitrary name of the render command
	(
		[this](FRHICommandListImmediate& RHICmdList) //Passed in variables
			{
			//This code block is ran inside of the Render Thread!
			//Which is why we need the reference to our FrontEnd class
			this->Calculate_RenderThread();
			}
		//This is now back in the Game Thread!
	);
}

void FGlobalComputeShader_Interface::Calculate_RenderThread()
{
	check(IsInRenderingThread());

	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);

	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	DispatchComputeShader(RHICmdList, *ComputeShader, 1, 1, 1);
}


	//Declare our shader:   ShaderClassType					ShaderFilePath										    Shader function name		Type
	IMPLEMENT_GLOBAL_SHADER(FGlobalComputeShader_Interface, "/Plugin/ComputeShaderDev/Private/WeatherShader.usf",      "simulateStep",       SF_Compute);


