// Fill out your copyright notice in the Description page of Project Settings.

#include "ShaderController.h"
#include "ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "SceneInterface.h"
#include "DynamicRHIResourceArray.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 1 

AShaderController::AShaderController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShaderController::OnConstruction(const FTransform& Transform)
{
	//This will be a for loop based on a blueprint available variable
	//Basically we will allow the user to feed in an array, and we will auto
	//fill the TArray_FStruct_Shader_CPU. Or we could expect the user to have already
	//filled TArray_FStruct_Shader_CPU in blueprints.
	TArray_FStruct_Shader_CPU.Empty();
	//Create our data struct
	FStruct_Shader_CPU instanceData;
	//Set our data struct values
	instanceData.runCount = 0;
	//Add our data struct to the array
	TArray_FStruct_Shader_CPU.Add(instanceData);
		 
}

void AShaderController::BeginPlay()
{
	//Init our instance of compute ComputeShaderInstance controller

	Shader_Constant_Params.ArrayNum = TArray_FStruct_Shader_CPU.Num();
		Shader_Variable_Params = FVariableParameters();

	Super::BeginPlay();
}

void AShaderController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShaderController::Compute(float DeltaTime)
{
		ExecuteComputeShader(TArray_FStruct_Shader_CPU, DeltaTime);
		//Fencing forces the game thread to wait for the render thread to finish
		ReleaseResourcesFence.BeginFence();
		ReleaseResourcesFence.Wait();
}


void AShaderController::ExecuteComputeShader(TArray<FStruct_Shader_CPU> &currentStates, float DeltaTime)
{
	//This is ran in the Game Thread!
	Shader_Variable_Params.DeltaTime = DeltaTime;

	//Everything sent here is passed by value, changes are lost
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		FComputeShaderRunner,		//TypeName - Arbitrary name of the render command
		AShaderController*,	//ParamType1 - The type of the parameter
		FrontEnd,					//ParamName1 - Name of the parameter inside the render command
		this,						//ParamValue1 - the value of the parameter passed from the outside
		TArray<FStruct_Shader_CPU>&, states, currentStates,		//ParamType/Name/Value 2
		{
			//This code block is ran inside of the Render Thread!
			//Which is why we need the reference to our FrontEnd class
			FrontEnd->ExecuteInRenderThread(states);
		}
		//This is now back in the Game Thread!
	);
}

//This can only run after begin play!
void AShaderController::ExecuteInRenderThread(TArray<FStruct_Shader_CPU> &currentStates)
{
	check(IsInRenderingThread());

	//Create TResourceArray, and fill it with out data
	//This should be the actual data sent to the GPU;
	TResourceArray<FStruct_Shader_CPU> FStruct_Shader_CPU_Data;
	for (int i = 0; i < Shader_Constant_Params.ArrayNum; i++) {
		FStruct_Shader_CPU_Data.Add(currentStates[i]);
	}
	//This lets the gpu know about our resource? Pointer?
	FRHIResourceCreateInfo resource;
	resource.ResourceArray = &FStruct_Shader_CPU_Data;
	//--------------------------------------------------------

	//Create various interfaces for our TResourceArray Data (resource)
	Interface_FStruct_Shader_GPU_Buffer = RHICreateStructuredBuffer(sizeof(FStruct_Shader_CPU), sizeof(FStruct_Shader_CPU) * Shader_Constant_Params.ArrayNum, BUF_UnorderedAccess | BUF_ShaderResource | 0, resource);
	Interface_FStruct_Shader_GPU_Buffer_UAV = RHICreateUnorderedAccessView(Interface_FStruct_Shader_GPU_Buffer, false, false);

	/* Get global RHI command list */
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//Declares our shader
	TShaderMapRef<FGlobalComputeShader> ComputeShaderInstance(GetGlobalShaderMap(GetWorld()->Scene->GetFeatureLevel()));

	//Creates an instance of our shader
	RHICmdList.SetComputeShader(ComputeShaderInstance->GetComputeShader());

	// Set ComputeShaderInstance inputs/outputs
	ComputeShaderInstance->BindInterfaceToShaderParamName(RHICmdList, Interface_FStruct_Shader_GPU_Buffer_UAV);
	ComputeShaderInstance->BintInterfaceToUniformBuffersParamName(RHICmdList, Shader_Constant_Params, Shader_Variable_Params);

	// Dispatch compute ComputeShaderInstance
	DispatchComputeShader(RHICmdList, *ComputeShaderInstance, 1, 1, 1);
	
	//Release buffers so another shader instance can use them (buffers are global thats why)
	ComputeShaderInstance->UnbindBuffers(RHICmdList);
	
	//Lock Interface_FStruct_Shader_GPU_Buffer to enable CPU read
	char* shaderData = (char*)RHICmdList.LockStructuredBuffer(Interface_FStruct_Shader_GPU_Buffer, 0, sizeof(FStruct_Shader_CPU) * Shader_Constant_Params.ArrayNum, EResourceLockMode::RLM_ReadOnly);
	
	//Copy the GPU data back to CPU side (&currentStates)
	FStruct_Shader_CPU* p = (FStruct_Shader_CPU*)shaderData;
	for (int32 Row = 0; Row < Shader_Constant_Params.ArrayNum; ++Row) {
		currentStates[Row] = *p;
		p++;
	}

	//Unlock Interface_FStruct_Shader_GPU_Buffer when finished
	RHICmdList.UnlockStructuredBuffer(Interface_FStruct_Shader_GPU_Buffer);
}