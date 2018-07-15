#pragma once
#include "WeatherSimulator/WeatherManager.h"
#include <algorithm> // for FMath::Min/max
#include "ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "SceneInterface.h"
#include "DynamicRHIResourceArray.h"

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
	TArray_FStruct_Columns_CPU_gridInit.Empty();
	TArray_FStruct_Columns_CPU_gridInit.Reserve(256);

	//Add our default data struct to the array
	for (int i = 0; i<256; i++)
	{
		TArray_FStruct_Columns_CPU_gridInit.Add(FStruct_Columns_CPU());
	}


//	Shader_Constant_Params.ArrayNum = TArray_FStruct_Columns_CPU_gridInit.Num();
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
	ExecuteComputeShader(TArray_FStruct_Columns_CPU_gridInit, DeltaTime);
	//Fencing forces the game thread to wait for the render thread to finish
	ReleaseResourcesFence.BeginFence();
	ReleaseResourcesFence.Wait();
}

void AWeatherManager::ExecuteComputeShader(TArray<FStruct_Columns_CPU> &currentStates, float DeltaTime)
{
	//This is ran in the Game Thread!
//	Shader_Variable_Params.DeltaTime = DeltaTime;

	//Everything sent here is passed by value, changes are lost
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		FComputeShaderRunner,		//TypeName - Arbitrary name of the render command
		AWeatherManager*,	//ParamType1 - The type of the parameter
		FrontEnd,					//ParamName1 - Name of the parameter inside the render command
		this,						//ParamValue1 - the value of the parameter passed from the outside
		TArray<FStruct_Columns_CPU>&, states, currentStates,		//ParamType/Name/Value 2
		{
			//This code block is ran inside of the Render Thread!
			//Which is why we need the reference to our FrontEnd class
			FrontEnd->ExecuteInRenderThread(states);
		}
		//This is now back in the Game Thread!
	);
}

//This can only run after begin play!
void AWeatherManager::ExecuteInRenderThread(TArray<FStruct_Columns_CPU> &currentStates)
{
	//This code block is ran inside of the Render Thread!
	check(IsInRenderingThread());



	//struct of information for a future resource on the GPU
	FRHIResourceCreateInfo FGPU_Resource_Info;

	//Create TResourceArray class
	//If we set bNeedsCPUAccess, we could remove the middleman (currentStates) and call Discard() ourselves ?
	//when we don't need it anymore (does this have to be render thread data?
	TResourceArray<FStruct_Columns_CPU> FStruct_Columns_CPU_Data;
	//FStruct_Columns_CPU_Data.SetAllowCPUAccess(true);

	//LETS SEE IF WE CAN SKIP THIS
	//Fill it with our data to be sent to the GPU
	for (int i = 0; i < 10; i++) {
		FStruct_Columns_CPU_Data.Add(currentStates[i]);
	}
	//AND RIGHT HERE JUST PASS THE REFERENCE TO OUR ARRAY!
	//Now put a reference to this data into our FGPU_Resource_Info class
	FGPU_Resource_Info.ResourceArray = &FStruct_Columns_CPU_Data;
	//FGPU_Resource_Info.ResourceArray = &FrontEnd->TArray_FStruct_Columns_CPU_gridInit

	//FResourceBulkDataInterface <- Allows for direct GPU mem allocation for bulk resource types.




	//Initializing the buffer and writing data to GPU
	//Create various interfaces for our TResourceArray Data (FGPU_Resource_Info)
	Interface_FStruct_Shader_GPU_Buffer = RHICreateStructuredBuffer(sizeof(FStruct_Columns_CPU), sizeof(FStruct_Columns_CPU) * 10, BUF_UnorderedAccess | BUF_ShaderResource | 0, FGPU_Resource_Info);
	//--------------------------------------------------------
	//At this point our TResourceArray (FStruct_Columns_CPU_Data) will destroy itself once its finished copying its data to gpu!
	//Unless bNeedsCPUAccess is set to true!
	Interface_FStruct_Shader_GPU_Buffer_UAV = RHICreateUnorderedAccessView(Interface_FStruct_Shader_GPU_Buffer, false, false);

	/* Get global RHI command list */
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();


	// Get the actual shader instances off the ShaderMap		// Get the collection of Global Shaders
	TShaderMapRef<FGlobalComputeShader> ComputeShaderInstance(GetGlobalShaderMap(GetWorld()->Scene->GetFeatureLevel()));

	// Call our function to set up parameters	
	ComputeShaderInstance->SetShaderParameters(RHICmdList, numSteps, dT,
		gridX, gridY, gridZ, gridXSize, gridYSize,
		currInd, simulationTime);

	//Creates an instance of our shader
	RHICmdList.SetComputeShader(ComputeShaderInstance->GetComputeShader());

	//// Set ComputeShaderInstance inputs/outputs
	////Mapping the interfaces to the names the shader expects
	//ComputeShaderInstance->BindDataInterfaceToShaderParamName(RHICmdList, Interface_FStruct_Shader_GPU_Buffer_UAV);
	//ComputeShaderInstance->BindDataInterfaceToUniformBuffersParamName(RHICmdList, Shader_Constant_Params, Shader_Variable_Params);

	// Dispatch compute ComputeShaderInstance
	DispatchComputeShader(RHICmdList, *ComputeShaderInstance, 1, 1, 1);

	//Release buffers so another shader instance can use them (buffers are global thats why)
	ComputeShaderInstance->UnbindBuffers(RHICmdList);

	//Lock Interface_FStruct_Shader_GPU_Buffer to enable CPU read
	char* shaderData = (char*)RHICmdList.LockStructuredBuffer(Interface_FStruct_Shader_GPU_Buffer, 0, sizeof(FStruct_Columns_CPU) * 10, EResourceLockMode::RLM_ReadOnly);

	//Copy the GPU data back to CPU side (&currentStates)
	FStruct_Columns_CPU* p = (FStruct_Columns_CPU*)shaderData;
	for (int32 Row = 0; Row < 10; ++Row) {
		currentStates[Row] = *p;
		p++;
	}

	//Unlock Interface_FStruct_Shader_GPU_Buffer when finished
	RHICmdList.UnlockStructuredBuffer(Interface_FStruct_Shader_GPU_Buffer);
}



