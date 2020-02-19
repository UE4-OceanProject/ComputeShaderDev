#pragma once
#include "WeatherManager.h"
#include "Shader_Interface.h"
#include "Engine.h"
#include "RenderGraphUtils.h"

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

	//A_output_buffer_.SafeRelease();
	//A_output_UAV_.SafeRelease();

	//B_output_buffer_.SafeRelease();
	//B_output_UAV_.SafeRelease();

	//C_output_buffer_.SafeRelease();
	//C_output_UAV_.SafeRelease();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AWeatherManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool AWeatherManager::Setup() {
	return true;
}

// According to some result, UniformBuffer does not seem to be kept saved even if UniformBuffer_MultiFrame flag is set...
//Try using  //BUF_KeepCPUAccessible in UAV setting
bool AWeatherManager::Calculate(
	/*  input */const float x,
	/* input */TArray<FStruct_AirGridContainer_CPU>& input,
	/* output */TArray<FStruct_AirGridContainer_CPU>& output
) {

	TArray<FWarpInConfig2> A_data = {
	{7,7,7,7,7,7,7,7,7,7},
	{7,7,7,7,7,7,7,7,7,7} };
	TArray<FWarpInConfig2> B_data = {
		{8,8,8,8,8,8,8,8,8,8},
		{8,8,8,8,8,8,8,8,8,8} };
	TArray<FWarpInConfig2> C_data = {
		{9,9,9,9,9,9,9,9,9,9},
		{9,9,9,9,9,9,9,9,9,9} };

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
	FGlobalComputeShader_Interface::FParameters Parameters;
	Parameters.test_outputA = A_output_UAV_;



	AWeatherManager* FrontEnd = this;
	ENQUEUE_RENDER_COMMAND(CalculateCommand)//TypeName - Arbitrary name of the render command
		(
			[FrontEnd, Parameters](FRHICommandListImmediate& RHICmdList) //Passed in variables
	{
		//This code block is ran inside of the Render Thread!
		//Which is why we need the reference to our FrontEnd class
		FrontEnd->Calculate_RenderThread(Parameters);
	}
			//This is now back in the Game Thread!
	);


	render_command_fence_.BeginFence();
	render_command_fence_.Wait(); // Waits for pending fence commands to retire.

	UE_LOG(LogTemp, Warning, TEXT("===== Calculate ====="));

	PrintResult(output);


	return true;
}





	void AWeatherManager::Calculate_RenderThread(FGlobalComputeShader_Interface::FParameters Parameters) 
	{
		check(IsInRenderingThread());


		TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

		TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);

		// Get global RHI command list
		FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();


		FComputeShaderUtils::Dispatch(RHICmdList, *ComputeShader, Parameters, FIntVector(1, 1, 1));
		//DispatchComputeShader(RHICmdList, *weather_compute_shader_, 1, 1, 1);


		//weather_compute_shader_->ClearOutput(RHICmdList);
		//weather_compute_shader_->ClearParameters(RHICmdList);

		//FWarpInConfig2* A_shader_data = (FWarpInConfig2*)RHICmdList.LockStructuredBuffer(A_output_buffer_, 0, sizeof(FWarpInConfig2) * num_input_, EResourceLockMode::RLM_ReadOnly);
		////FMemory::Memcpy(output->GetData(), shader_data2, sizeof(FWarpInConfig2) * num_input_);

		//FWarpInConfig2* B_shader_data = (FWarpInConfig2*)RHICmdList.LockStructuredBuffer(B_output_buffer_, 0, sizeof(FWarpInConfig2) * num_input_, EResourceLockMode::RLM_ReadOnly);
		////FMemory::Memcpy(output->GetData(), shader_data2, sizeof(FWarpInConfig2) * num_input_);

		//FWarpInConfig2* C_shader_data = (FWarpInConfig2*)RHICmdList.LockStructuredBuffer(C_output_buffer_, 0, sizeof(FWarpInConfig2) * num_input_, EResourceLockMode::RLM_ReadOnly);
		////FMemory::Memcpy(output->GetData(), shader_data2, sizeof(FWarpInConfig2) * num_input_);

		//TArray<FWarpInConfig2> A_data = {
		//	{7,7,7,7,7,7,7,7,7,7},
		//	{7,7,7,7,7,7,7,7,7,7} };
		//TArray<FWarpInConfig2> B_data = {
		//	{8,8,8,8,8,8,8,8,8,8},
		//	{8,8,8,8,8,8,8,8,8,8} };
		//TArray<FWarpInConfig2> C_data = {
		//	{9,9,9,9,9,9,9,9,9,9},
		//	{9,9,9,9,9,9,9,9,9,9} };


		//FMemory::Memcpy(A_data.GetData(), A_shader_data, sizeof(FWarpInConfig2) * num_input_);
		//FMemory::Memcpy(B_data.GetData(), B_shader_data, sizeof(FWarpInConfig2) * num_input_);
		//FMemory::Memcpy(C_data.GetData(), C_shader_data, sizeof(FWarpInConfig2) * num_input_);

		//FMemory::Memcpy(A_shader_data, A_data.GetData(), sizeof(FWarpInConfig2) * num_input_);
		//FMemory::Memcpy(B_shader_data, B_data.GetData(), sizeof(FWarpInConfig2) * num_input_);
		//FMemory::Memcpy(C_shader_data, C_data.GetData(), sizeof(FWarpInConfig2) * num_input_);

		//RHICmdList.UnlockStructuredBuffer(A_output_buffer_);
		//RHICmdList.UnlockStructuredBuffer(B_output_buffer_);
		//RHICmdList.UnlockStructuredBuffer(C_output_buffer_);
	}



// TResourceArray's values are still alive...
void AWeatherManager::PrintResult(const TArray<FStruct_AirGridContainer_CPU>& output) {
	//for (int32 index = 0; index < num_input_; ++index) {
	//	UE_LOG(LogTemp, Warning, TEXT("(%f, %f, %f)"),
	//		output[index].X, output[index].Y, output[index].Z);
	//}
}



