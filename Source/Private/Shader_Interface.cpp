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

//Change this back when fix is in place
//IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FGlobalWeatherParameters, "WeatherData");
//typedef TUniformBufferRef<FGlobalWeatherParameters> FUniformBufferRef;


//This wont be needed once fix is in.
FGlobalComputeShader_Interface::FGlobalComputeShader_Interface(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer) {
	A_output_.Bind(Initializer.ParameterMap, TEXT("test_outputA"), SPF_Mandatory);//x5600x3
}

//This wont be needed once fix is in.
bool FGlobalComputeShader_Interface::Serialize(FArchive& Ar) {
	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
	Ar
		<< A_output_ //x5600x3
		;
	return bShaderHasOutdatedParameters;
}

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




	// In this sample code, output_buffer_ has not input values, so what we need here is just the pointer to output_resource_.
	A_output_RA_.SetNum(num_input_);
	A_output_RA_.Init({ 7,7,7,7,7,7,7,7,7,7 }, 2);

	A_output_resource_.ResourceArray = &A_output_RA_;
	A_output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, A_output_resource_);
	A_output_UAV_ = RHICreateUnorderedAccessView(A_output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);


	//FGlobalComputeShader_Interface::
	//FGlobalComputeShader_Interface::FParameters MainWeatherParams; //Change this back when fix is in place


	//FGlobalWeatherParameters GlobalWeatherParams; //Change this back when fix is in place


	//GlobalWeatherParams.test_outputA = GlobalWeatherParams.test_outputA = A_output_UAV_; //Change this back when fix is in place

	//Must be done on render thread
	//MainWeatherParams.GlobalWeatherParameters = FUniformBufferRef::CreateUniformBufferImmediate(GlobalWeatherParams, UniformBuffer_MultiFrame); //Change this back when fix is in place
	//or try this (guys said above code is bad, because reference will dissapear when scope exit
	//MainWeatherParams.test_outputA = GlobalWeatherParams.test_outputA = A_output_UAV_;; //Change this back when fix is in place


	//MainWeatherParams.test_outputA = A_output_UAV_; //Change this back when fix is in place //Change this back when fix is in place

	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);

	// Get global RHI command list
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	RHICmdList.SetComputeShader(ComputeShader->GetComputeShader());//Change this back when fix is in place??

	TArray<FShaderResourceParameter>RotateableBufers = { {A_output_} };//Change this back when fix is in place??
	RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[0].GetBaseIndex(), A_output_UAV_);//Change this back when fix is in place??

	DispatchComputeShader(RHICmdList, *ComputeShader, 1, 1, 1); 
	//FComputeShaderUtils::Dispatch(RHICmdList, *ComputeShader, MainWeatherParams, FIntVector(1, 1, 1));//Change this back when fix is in place

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
	RHICmdList.SetComputeShader(ComputeShader->GetComputeShader());
	TArray<FShaderResourceParameter>RotateableBufers = { {A_output_} };//Still needed or we don't get a buffer in our shader // even though we are not setting the data. 
	RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[0].GetBaseIndex(), A_output_UAV_);//Still needed or we don't get a buffer in our shader // even though we are not setting the data. 
	DispatchComputeShader(RHICmdList, *ComputeShader, 1, 1, 1);
}


	//Declare our shader:   ShaderClassType					ShaderFilePath										    Shader function name		Type
	IMPLEMENT_GLOBAL_SHADER(FGlobalComputeShader_Interface, "/Plugin/ComputeShaderDev/Private/WeatherShader.usf",      "simulateStep",       SF_Compute);


