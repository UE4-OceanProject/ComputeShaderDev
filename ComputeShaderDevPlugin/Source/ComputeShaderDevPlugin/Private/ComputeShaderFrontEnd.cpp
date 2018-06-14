// Fill out your copyright notice in the Description page of Project Settings.

#include "ComputeShaderFrontEnd.h"
#include "ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "SceneInterface.h"
#include "DynamicRHIResourceArray.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 1 

AComputeShaderFrontEnd::AComputeShaderFrontEnd()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AComputeShaderFrontEnd::OnConstruction(const FTransform& Transform)
{
	//This will be a for loop based on a blueprint available variable
	//Basically we will allow the user to feed in an array, and we will auto
	//fill the WeatherPointStates. Or we could expect the user to have already
	//filled WeatherPointStates in blueprints.
	CS_WeatherPointStates.Empty();
	//Create our data struct
	FWeatherXYZPointState gpustate;
	//Set our data struct values
	gpustate.instanceId = 1;
	//Add our data struct to the array
	CS_WeatherPointStates.Add(gpustate);
		 
}

void AComputeShaderFrontEnd::BeginPlay()
{
	//Init our instance of compute shader controller

	CS_ConstantParameters.WeatherPointsCount = CS_WeatherPointStates.Num();
		CS_VariableParameters = FVariableParameters();

	Super::BeginPlay();
}

void AComputeShaderFrontEnd::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AComputeShaderFrontEnd::Compute(float DeltaTime)
{
		ExecuteComputeShader(CS_WeatherPointStates, DeltaTime);
		//Fencing forces the game thread to wait for the render thread to finish
		ReleaseResourcesFence.BeginFence();
		ReleaseResourcesFence.Wait();
}


void AComputeShaderFrontEnd::ExecuteComputeShader(TArray<FWeatherXYZPointState> &currentStates, float DeltaTime)
{
	//This is ran in the Game Thread!
	
	CS_VariableParameters.DeltaTime = DeltaTime;
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		FComputeShaderRunner,		//TypeName - Arbitrary name of the render command
		AComputeShaderFrontEnd*,	//ParamType1 - The type of the parameter
		FrontEnd,					//ParamName1 - Name of the parameter inside the render command
		this,						//ParamValue1 - the value of the parameter passed from the outside
		TArray<FWeatherXYZPointState>&, states, currentStates,		//ParamType/Name/Value 2
		{
			//This code block is ran inside of the Render Thread!
			//Which is why we need the referenec to our FrontEnd class
			FrontEnd->ExecuteInRenderThread(states);
		}
		//This is now back in the Game Thread!
	);
}

//This can only run after begin play!
void AComputeShaderFrontEnd::ExecuteInRenderThread(TArray<FWeatherXYZPointState> &currentStates)
{
	check(IsInRenderingThread());

	//This is our bound data struct for use inside the shader
	//Data is filled in here, before being sent over
	TResourceArray<FWeatherXYZPointState> data;
	for (int i = 0; i < CS_ConstantParameters.WeatherPointsCount; i++) {
		data.Add(currentStates[i]);
	}
	FRHIResourceCreateInfo resource;
	resource.ResourceArray = &data;
	//--------------------------------------------------------

	FStructuredBufferRHIRef buffer = RHICreateStructuredBuffer(sizeof(FWeatherXYZPointState), sizeof(FWeatherXYZPointState) * CS_ConstantParameters.WeatherPointsCount, BUF_UnorderedAccess | 0, resource);
	FUnorderedAccessViewRHIRef uav = RHICreateUnorderedAccessView(buffer, false, false);

	/* Get global RHI command list */
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
	TShaderMapRef<FComputeShaderDevPluginModule> shader(GetGlobalShaderMap(GetWorld()->Scene->GetFeatureLevel()));
	//-------------------------------

	RHICmdList.SetComputeShader(shader->GetComputeShader());

	// Set shader inputs/outputs
	shader->SetSurfaces(RHICmdList, uav);
	shader->SetUniformBuffers(RHICmdList, CS_ConstantParameters, CS_VariableParameters);

	// Dispatch compute shader
	DispatchComputeShader(RHICmdList, *shader, 1, 1, 1);

	//Release buffers so another shader can use them (buffers are global thats why)
	shader->UnbindBuffers(RHICmdList);
	
	//Lock buffer to enable CPU read
	char* shaderData = (char*)RHICmdList.LockStructuredBuffer(buffer, 0, sizeof(FWeatherXYZPointState) * CS_ConstantParameters.WeatherPointsCount, EResourceLockMode::RLM_ReadOnly);
	
	//Copy the GPU data back to CPU side (&currentStates)
	FWeatherXYZPointState* p = (FWeatherXYZPointState*)shaderData;
	for (int32 Row = 0; Row < CS_ConstantParameters.WeatherPointsCount; ++Row) {
		currentStates[Row] = *p;
		p++;
	}

	//Unlock buffer when finished
	RHICmdList.UnlockStructuredBuffer(buffer);
}