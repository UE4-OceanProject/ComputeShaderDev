#pragma once
#include "WeatherSimulator/WeatherManager.h"
#include "DynamicRHIResourceArray.h"
#include "Engine.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 1 


AWeatherManager::AWeatherManager(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	//	PrimaryActorTick.bCanEverTick = true;
}

void AWeatherManager::OnConstruction(const FTransform& Transform)
{
}

void AWeatherManager::BeginPlay()
{
	//Init our instance of compute ComputeShaderInstance controller
	TArray_FStruct_AirGridContainer_CPU_gridInit.Empty();
	TArray_FStruct_AirGridContainer_CPU_gridInit.Reserve(256);

	//Add our default data struct to the array
	for (int i = 0; i<256; i++)
	{
		TArray_FStruct_AirGridContainer_CPU_gridInit.Add(FStruct_AirGridContainer_CPU());
	}


//	Shader_Constant_Params.ArrayNum = TArray_FStruct_AirGridContainer_CPU_gridInit.Num();
	//Shader_Constant_Params = FShaderConstants_Class();
	//Shader_Variable_Params = FVariables_Class();

	Super::BeginPlay();
}

void AWeatherManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeatherManager::Compute(float DeltaTime)
{

		ExecuteComputeShader(TArray_FStruct_AirGridContainer_CPU_gridInit, DeltaTime);
		//Fencing forces the game thread to wait for the render thread to finish
		ReleaseResourcesFence.BeginFence();
		ReleaseResourcesFence.Wait();

}

void AWeatherManager::ExecuteComputeShader(TArray<FStruct_AirGridContainer_CPU> &currentStates, float DeltaTime)
{
	//This is ran in the Game Thread!
//	Shader_Variable_Params.DeltaTime = DeltaTime;

	//Everything sent here is passed by value, changes are lost
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		FComputeShaderRunner,		//TypeName - Arbitrary name of the render command
		AWeatherManager*,	//ParamType1 - The type of the parameter
		FrontEnd,					//ParamName1 - Name of the parameter inside the render command
		this,						//ParamValue1 - the value of the parameter passed from the outside
		TArray<FStruct_AirGridContainer_CPU>&, states, currentStates,		//ParamType/Name/Value 2
		{
			//This code block is ran inside of the Render Thread!
			//Which is why we need the reference to our FrontEnd class
			FrontEnd->ExecuteInRenderThread(states);
		}
		//This is now back in the Game Thread!
	);
}

//This can only run after begin play!
void AWeatherManager::ExecuteInRenderThread(TArray<FStruct_AirGridContainer_CPU> &currentStates)
{
	//This code block is ran inside of the Render Thread!
	check(IsInRenderingThread());

	/* Get global RHI command list */
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//Creates an instance of our shader interface
	// Get the actual shader instances off the ShaderMap		// Get the collection of Global Shaders
	TShaderMapRef<FGlobalComputeShader> ComputeShaderInstance(GetGlobalShaderMap(GetWorld()->Scene->GetFeatureLevel()));

	//Set our instance of our shader into the RHICmdList
	RHICmdList.SetComputeShader(ComputeShaderInstance->GetComputeShader());

	// Call our function to set up parameters	
	ComputeShaderInstance->SetShaderParameters(RHICmdList, numSteps, dT,
		gridX, gridY, gridZ, gridXSize, gridYSize,
		simulationTime,
		currInd,
		gridSizeK,
		ground,
		gridRslow,
		gridInit,
		Grid3D0);


	//// Set ComputeShaderInstance inputs/outputs
	////Mapping the interfaces to the names the shader expects
	//ComputeShaderInstance->BindDataInterfaceToShaderParamName(RHICmdList, Interface_FStruct_Shader_CPU_Buffer_UAV);
//	ComputeShaderInstance->BindDataInterfaceToShaderParamName(RHICmdList, Interface_FStruct_AirGridContainer_grid3D_CPU_Data_Buffer_UAV);
	//ComputeShaderInstance->BindDataInterfaceToUniformBuffersParamName(RHICmdList, Shader_Constant_Params, Shader_Variable_Params);

	// Dispatch compute ComputeShaderInstance
	DispatchComputeShader(RHICmdList, *ComputeShaderInstance, 1, 1, 1);
	currInd = (currInd + 1) % 3;

	//Release buffers so another shader instance can use them (buffers are global thats why)
//	ComputeShaderInstance->UnbindBuffers(RHICmdList);

	//Lock Interface_FStruct_Shader_CPU_Buffer to enable CPU read
	//char* shaderData = (char*)RHICmdList.LockStructuredBuffer(Interface_FStruct_Shader_CPU_Buffer, 0, sizeof(FStruct_AirGridContainer_CPU) * 10, EResourceLockMode::RLM_ReadOnly);

	//Copy the CPU data back to CPU side (&currentStates)
	//FStruct_AirGridContainer_CPU* p = (FStruct_AirGridContainer_CPU*)shaderData;
	//for (int32 Row = 0; Row < 10; ++Row) {
	//	currentStates[Row] = *p;
	//	p++;
	//}

	//Unlock Interface_FStruct_Shader_CPU_Buffer when finished
	//RHICmdList.UnlockStructuredBuffer(Interface_FStruct_Shader_CPU_Buffer);
}



