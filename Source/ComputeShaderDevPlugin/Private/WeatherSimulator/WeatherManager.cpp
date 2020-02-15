#pragma once
#include "WeatherSimulator/WeatherManager.h"
#include "DynamicRHIResourceArray.h"
#include "Engine.h"
//#include "RenderCaptureInterface.h"

//#define NUM_THREADS_PER_GROUP_DIMENSION 1 

// Sets default values
AWeatherManager::AWeatherManager(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWeatherManager::OnConstruction(const FTransform& Transform)
{
}

void AWeatherManager::BeginPlay()
{
	//Init our instance of compute ComputeShaderInstance controller
	//TArray_FStruct_AirGridContainer_CPU_gridInit.Empty();
	//TArray_FStruct_AirGridContainer_CPU_gridInit.Reserve(256);

	//Add our default data struct to the array
	//for (int i = 0; i<256; i++)
	//{
	//	TArray_FStruct_AirGridContainer_CPU_gridInit.Add(FStruct_AirGridContainer_CPU());
	//}


//	Shader_Constant_Params.ArrayNum = TArray_FStruct_AirGridContainer_CPU_gridInit.Num();
	//Shader_Constant_Params = FShaderConstants_Class();
	//Shader_Variable_Params = FVariables_Class();

  // Has to be initialized at BeginPlay(), instead of the class's constructor.
	ERHIFeatureLevel::Type shader_feature_level_test = GetWorld()->Scene->GetFeatureLevel();
	UE_LOG(LogTemp, Warning, TEXT("Shader Feature Level: %d"), shader_feature_level_test);
	UE_LOG(LogTemp, Warning, TEXT("Max Shader Feature Level: %d"), GMaxRHIFeatureLevel);

	Super::BeginPlay();
}

void AWeatherManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	// I'm not sure where is the appropriate place to call the following SafeRelease methods.
	// Destructor? EndPlay? BeginDestroy??
	//FStruct_Cell_gridSizeK_CPU_ResourceParameter_buffer_.SafeRelease();
	//FStruct_Cell_gridSizeK_CPU_ResourceParameter_SRV_.SafeRelease();

	//FStruct_GroundGridContainer_ground_CPU_ResourceParameter_buffer_.SafeRelease();
	//FStruct_GroundGridContainer_ground_CPU_ResourceParameter_SRV_.SafeRelease();

	//FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_buffer_.SafeRelease();
	//FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_SRV_.SafeRelease();

	//FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_buffer_.SafeRelease();
	//FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_SRV_.SafeRelease();

	output_buffer_.SafeRelease();
	output_UAV_.SafeRelease();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AWeatherManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}



void AWeatherManager::Compute(float DeltaTime)
{

		//ExecuteComputeShader(grid0Var, DeltaTime);
		////Fencing forces the game thread to wait for the render thread to finish
		//render_command_fence_.BeginFence();
		//render_command_fence_.Wait();

}

void AWeatherManager::ExecuteComputeShader(TArray<FStruct_AirGridContainer_CPU> &currentStates, float DeltaTime)
{
	//This is ran in the Game Thread!
//	Shader_Variable_Params.DeltaTime = DeltaTime;

	//Everything sent here is passed by value, changes are lost (4.22, is this still true?)
	//AWeatherManager* FrontEnd = this;
	//TArray<FStruct_AirGridContainer_CPU>* states = &currentStates;
	//ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)//TypeName - Arbitrary name of the render command
	//	(
	//		[FrontEnd, states](FRHICommandListImmediate& RHICmdList) //Passed in variables

	//		{
				//This code block is ran inside of the Render Thread!
				//Which is why we need the reference to our FrontEnd class
			//	FrontEnd->ExecuteInRenderThread(*states);
			//}
			//This is now back in the Game Thread!
		//);
}

//This can only run after begin play!
void AWeatherManager::ExecuteInRenderThread(TArray<FStruct_AirGridContainer_CPU> &currentStates)
{
	//This code block is ran inside of the Render Thread!
	//check(IsInRenderingThread());

	/* Get global RHI command list */
	//FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//Creates an instance of our shader interface
	// Get the actual shader instances off the ShaderMap		// Get the collection of Global Shaders
	//TShaderMapRef<FGlobalComputeShader_Interface> ComputeShaderInstance(GetGlobalShaderMap(GetWorld()->Scene->GetFeatureLevel()));

	//Set our instance of our shader into the RHICmdList
	//RHICmdList.SetComputeShader(ComputeShaderInstance->GetComputeShader());

	// Call our function to set up parameters	
	//FStructuredBufferRHIRef Interface_FStruct_Shader_CPU_Buffer = ComputeShaderInstance->SetShaderParameters(RHICmdList,
	//	dT,
	//	gridX, gridY, gridZ, gridXSize, gridYSize,
	//	simulationTime,
	//	prevGC, currGC, nextGC;


	//// Set ComputeShaderInstance inputs/outputs
	////Mapping the interfaces to the names the shader expects
	//ComputeShaderInstance->BindDataInterfaceToShaderParamName(RHICmdList, Interface_FStruct_Shader_CPU_Buffer_UAV);
//	ComputeShaderInstance->BindDataInterfaceToShaderParamName(RHICmdList, Interface_FStruct_AirGridContainer_grid3D_CPU_Data_Buffer_UAV);
	//ComputeShaderInstance->BindDataInterfaceToUniformBuffersParamName(RHICmdList, Shader_Constant_Params, Shader_Variable_Params);

	// Dispatch compute ComputeShaderInstance



	
//	Here we rotate the past current and future values in our simulation
	//for (int nS = 0; nS < numSteps; nS++)
	//{
	//	nextGC = (currGC + 1) % 3; //3 number of time array
	//	prevGC = (currGC - 1);

	//	if (prevGC < 0)
	//		prevGC = 2; // Set the last step.

		// Run simulation.
		//DispatchComputeShader(RHICmdList, *ComputeShaderInstance, 1, 1, 1);



	//Release buffers so another shader instance can use them (buffers are global thats why)
	//ComputeShaderInstance->UnbindBuffers(RHICmdList);

	//Lock Interface_FStruct_Shader_CPU_Buffer to enable CPU read
	//char* shaderData = (char*)RHICmdList.LockStructuredBuffer(Interface_FStruct_Shader_CPU_Buffer, 0/*5600 * 40 * currGC*/, 5600*40, EResourceLockMode::RLM_ReadOnly);
	
	//Copy the CPU data back to CPU side (&currentStates)
	//FStruct_AirGridContainer_CPU* p = (FStruct_AirGridContainer_CPU*)shaderData;
	//currentStates[0] = *p;
	//for (int32 Row = 0; Row < 3; ++Row) {
	//	currentStates[0] = *p;
	//	p++;
	//}



		//currGC = (currGC + 1) % 3;
	//}

	//Unlock Interface_FStruct_Shader_CPU_Buffer when finished
	//RHICmdList.UnlockStructuredBuffer(Interface_FStruct_Shader_CPU_Buffer);
}


















//These are resources we send to the GPU, we don't plan on getting these back from the GPU
bool AWeatherManager::UploadResourceParametersToShader(
	/*  input */
	const TArray<FStruct_Cell_CPU>& gridSizeK_,
	const TArray<FStruct_GroundGridContainer_CPU>& ground_,
	const TArray<FStruct_AirGridContainer_CPU>& gridRslow_,
	const TArray<FStruct_AirGridContainer_CPU>& gridInit_,
	const TArray<FStruct_AirGridContainer_CPU>& grid3D_
) {

	//if (gridSizeK_.Num() == 0) {
	//	UE_LOG(LogTemp, Warning, TEXT("Error: gridSizeK_ is empty at ATestComputeShaderActor::InitializeInputPosition."));
	//	return false;
	//}
	//gridSizeK_num_input_ = gridSizeK_.Num();

	//// We need to copy TArray to TResourceArray to set RHICreateStructuredBuffer.
	//FStruct_Cell_gridSizeK_CPU_ResourceParameter_RA_.SetNum(gridSizeK_num_input_);
	//FMemory::Memcpy(FStruct_Cell_gridSizeK_CPU_ResourceParameter_RA_.GetData(), gridSizeK_.GetData(), sizeof(FStruct_Cell_CPU) * gridSizeK_num_input_);

	//FStruct_Cell_gridSizeK_CPU_ResourceParameter_resource_.ResourceArray = &FStruct_Cell_gridSizeK_CPU_ResourceParameter_RA_;
	//// Note: In D3D11StructuredBuffer.cpp, ResourceArray->Discard() function is called, but not discarded??
	//FStruct_Cell_gridSizeK_CPU_ResourceParameter_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_Cell_CPU), sizeof(FStruct_Cell_CPU) * gridSizeK_num_input_, BUF_ShaderResource, FStruct_Cell_gridSizeK_CPU_ResourceParameter_resource_);
	//FStruct_Cell_gridSizeK_CPU_ResourceParameter_SRV_ = RHICreateShaderResourceView(FStruct_Cell_gridSizeK_CPU_ResourceParameter_buffer_);



	//if (ground_.Num() == 0) {
	//	UE_LOG(LogTemp, Warning, TEXT("Error: gridSizeK_ is empty at ATestComputeShaderActor::InitializeInputPosition."));
	//	return false;
	//}
	//ground_num_input_ = ground_.Num();

	//// We need to copy TArray to TResourceArray to set RHICreateStructuredBuffer.
	//FStruct_GroundGridContainer_ground_CPU_ResourceParameter_RA_.SetNum(ground_num_input_);
	//FMemory::Memcpy(FStruct_GroundGridContainer_ground_CPU_ResourceParameter_RA_.GetData(), ground_.GetData(), sizeof(FStruct_GroundGridContainer_CPU) * ground_num_input_);

	//FStruct_GroundGridContainer_ground_CPU_ResourceParameter_resource_.ResourceArray = &FStruct_GroundGridContainer_ground_CPU_ResourceParameter_RA_;
	//// Note: In D3D11StructuredBuffer.cpp, ResourceArray->Discard() function is called, but not discarded??
	//FStruct_GroundGridContainer_ground_CPU_ResourceParameter_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_GroundGridContainer_CPU), sizeof(FStruct_GroundGridContainer_CPU) * ground_num_input_, BUF_ShaderResource, FStruct_GroundGridContainer_ground_CPU_ResourceParameter_resource_);
	//FStruct_GroundGridContainer_ground_CPU_ResourceParameter_SRV_ = RHICreateShaderResourceView(FStruct_GroundGridContainer_ground_CPU_ResourceParameter_buffer_);




	//if (gridRslow_.Num() == 0) {
	//	UE_LOG(LogTemp, Warning, TEXT("Error: gridSizeK_ is empty at ATestComputeShaderActor::InitializeInputPosition."));
	//	return false;
	//}
	//gridRslow_num_input_ = gridRslow_.Num();

	//// We need to copy TArray to TResourceArray to set RHICreateStructuredBuffer.
	//FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_RA_.SetNum(gridRslow_num_input_);
	//FMemory::Memcpy(FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_RA_.GetData(), gridRslow_.GetData(), sizeof(FStruct_AirGridContainer_CPU) * gridRslow_num_input_);

	//FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_resource_.ResourceArray = &FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_RA_;
	//// Note: In D3D11StructuredBuffer.cpp, ResourceArray->Discard() function is called, but not discarded??
	//FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirGridContainer_CPU), sizeof(FStruct_AirGridContainer_CPU) * gridRslow_num_input_, BUF_ShaderResource, FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_resource_);
	//FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_SRV_ = RHICreateShaderResourceView(FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_buffer_);



	//if (gridInit_.Num() == 0) {
	//	UE_LOG(LogTemp, Warning, TEXT("Error: gridSizeK_ is empty at ATestComputeShaderActor::InitializeInputPosition."));
	//	return false;
	//}
	//gridInit_num_input_ = gridInit_.Num();

	//// We need to copy TArray to TResourceArray to set RHICreateStructuredBuffer.
	//FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_RA_.SetNum(gridInit_num_input_);
	//FMemory::Memcpy(FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_RA_.GetData(), gridInit_.GetData(), sizeof(FStruct_AirGridContainer_CPU) * gridInit_num_input_);

	//FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_resource_.ResourceArray = &FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_RA_;
	//// Note: In D3D11StructuredBuffer.cpp, ResourceArray->Discard() function is called, but not discarded??
	//FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_buffer_ = RHICreateStructuredBuffer(sizeof(FStruct_AirGridContainer_CPU), sizeof(FStruct_AirGridContainer_CPU) * gridInit_num_input_, BUF_ShaderResource, FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_resource_);
	//FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_SRV_ = RHICreateShaderResourceView(FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_buffer_);


	return true;
}



void AWeatherManager::SetUniformBuffersInShader(
	/*  input */const float y, const float z) {
	//This is ran in the Game Thread!
	//	Shader_Variable_Params.DeltaTime = DeltaTime;
	offset_.Y = y;
	offset_.Z = z;

	AWeatherManager* FrontEnd = this;


	ENQUEUE_RENDER_COMMAND(SetUniformBuffersInShaderCommand)//TypeName - Arbitrary name of the render command
	(
		[FrontEnd, y, z](FRHICommandListImmediate& RHICmdList) //Passed in variables

		{
			//This code block is ran inside of the Render Thread!
			//Which is why we need the reference to our FrontEnd class
			FrontEnd->SetUniformBuffersInShader_RenderThread(y, z);
		}
		//This is now back in the Game Thread!
	);

	render_command_fence_.BeginFence();
	render_command_fence_.Wait();
}




void AWeatherManager::SetUniformBuffersInShader_RenderThread(
	/*  input */const float y, const float z) {
	check(IsInRenderingThread());

	// Get global RHI command list
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	// Get the actual shader instance off the ShaderMap
	TShaderMapRef<FGlobalComputeShader_Interface> weather_compute_shader_(shader_map);

	RHICmdList.SetComputeShader(weather_compute_shader_->GetComputeShader());
	weather_compute_shader_->SetUniformBuffers(RHICmdList, y, z);
}



// According to some result, UniformBuffer does not seem to be kept saved even if UniformBuffer_MultiFrame flag is set...
//Try using  //BUF_KeepCPUAccessible in UAV setting
bool AWeatherManager::Calculate(
	/*  input */const float x,
	/* input */TArray<FStruct_AirGridContainer_CPU>& input,
	/* output */TArray<FStruct_AirGridContainer_CPU>& output
) {


	//if (gridSizeK_num_input_ == 0 || ground_num_input_ == 0 || gridRslow_num_input_ == 0 || gridInit_num_input_ == 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Error: input grids have not been set correctly at AWeatherManager::Calculate."));
	//	return false;
	//}
	TArray<FWarpInConfig2> data = { 
		{7,7,7,7,7,7,7,7,7,7},
		{7,7,7,7,7,7,7,7,7,7} };




	// In this sample code, output_buffer_ has not input values, so what we need here is just the pointer to output_resource_.
	output_RA_.SetNum(num_input_);
	FMemory::Memcpy(output_RA_.GetData(), data.GetData(), sizeof(FWarpInConfig2) * num_input_);
	output_resource_.ResourceArray = &output_RA_;
	output_buffer_ = RHICreateStructuredBuffer(sizeof(FWarpInConfig2), sizeof(FWarpInConfig2) * num_input_, BUF_ShaderResource | BUF_UnorderedAccess, output_resource_);
	output_UAV_ = RHICreateUnorderedAccessView(output_buffer_, /* bool bUseUAVCounter */ false, /* bool bAppendBuffer */ false);



	//Don't need to do this, as its already inited by BP
	//output.SetNum(grid3D_num_input_);
	offset_.X = x;
	FVector offset = offset_;
	bool yz_updated = false;


	AWeatherManager* FrontEnd = this;

	TArray<FStruct_AirGridContainer_CPU>* output__ = &output;


	output.SetNum(num_input_);
	ENQUEUE_RENDER_COMMAND(CalculateCommand)//TypeName - Arbitrary name of the render command
	(
		[FrontEnd, offset, yz_updated, output__](FRHICommandListImmediate& RHICmdList) //Passed in variables
		{
			//This code block is ran inside of the Render Thread!
			//Which is why we need the reference to our FrontEnd class
			FrontEnd->Calculate_RenderThread(offset, yz_updated, output__);
		}
		//This is now back in the Game Thread!
	);


	render_command_fence_.BeginFence();
	render_command_fence_.Wait(); // Waits for pending fence commands to retire.

	UE_LOG(LogTemp, Warning, TEXT("===== Calculate ====="));

	PrintResult(output);
	return true;
}


void AWeatherManager::Calculate_RenderThread(
	/*  input */const FVector xyz, const bool yz_updated,
	/* output */TArray<FStruct_AirGridContainer_CPU>* output) {
	check(IsInRenderingThread());


	// Get global RHI command list
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	// Get the actual shader instance off the ShaderMap
	TShaderMapRef<FGlobalComputeShader_Interface> weather_compute_shader_(shader_map);

	RHICmdList.SetComputeShader(weather_compute_shader_->GetComputeShader());

	//Not using the uniform buffer right now
	//weather_compute_shader_->SetShaderParameters(RHICmdList, xyz.X);
	//if (yz_updated) {
	//	weather_compute_shader_->SetUniformBuffers(RHICmdList, xyz.Y, xyz.Z);
	//}

	weather_compute_shader_->SetShaderResourceParameters(RHICmdList
		//FStruct_Cell_gridSizeK_CPU_ResourceParameter_SRV_,
		//FStruct_GroundGridContainer_ground_CPU_ResourceParameter_SRV_,
		//FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_SRV_,
		//FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_SRV_
		);

	//RenderCaptureInterface::FScopedCapture
	weather_compute_shader_->SetOutput(RHICmdList, output_UAV_);


	DispatchComputeShader(RHICmdList, *weather_compute_shader_, 1, 1, 1);

	weather_compute_shader_->ClearOutput(RHICmdList);
	weather_compute_shader_->ClearParameters(RHICmdList);

	FWarpInConfig2* shader_data2 = (FWarpInConfig2*)RHICmdList.LockStructuredBuffer(output_buffer_, 0, sizeof(FWarpInConfig2) * num_input_, EResourceLockMode::RLM_ReadOnly);
	//FMemory::Memcpy(output->GetData(), shader_data2, sizeof(FWarpInConfig2) * num_input_);


	TArray<FWarpInConfig2> data = {
	{7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7} };


	FMemory::Memcpy(data.GetData(), shader_data2, sizeof(FWarpInConfig2) * num_input_);
	FMemory::Memcpy(shader_data2, data.GetData(), sizeof(FWarpInConfig2) * num_input_);

	RHICmdList.UnlockStructuredBuffer(output_buffer_);

}

// TResourceArray's values are still alive...
void AWeatherManager::PrintResult(const TArray<FStruct_AirGridContainer_CPU>& output) {
	//for (int32 index = 0; index < num_input_; ++index) {
	//	UE_LOG(LogTemp, Warning, TEXT("(%f, %f, %f)"),
	//		output[index].X, output[index].Y, output[index].Z);
	//}
}

