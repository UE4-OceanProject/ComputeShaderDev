#pragma once
#include "WeatherManager.h"
#include "Shader_Interface.h"

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
	Super::BeginPlay();
}

void AWeatherManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {


	//C_output_buffer_.SafeRelease();
	//C_output_UAV_.SafeRelease();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AWeatherManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool AWeatherManager::Setup() 
{
	// Get global RHI command list
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	TShaderMapRef< FGlobalComputeShader_Interface > weather_compute_shader_(GlobalShaderMap);


	weather_compute_shader_->SetParameters();


	return true;
}

// According to some result, UniformBuffer does not seem to be kept saved even if UniformBuffer_MultiFrame flag is set...
//Try using  //BUF_KeepCPUAccessible in UAV setting
bool AWeatherManager::Calculate(
	/*  input */const float x,
	/* input */TArray<FStruct_AirGridContainer_CPU>& input,
	/* output */TArray<FStruct_AirGridContainer_CPU>& output
) 
{
	// Get global RHI command list
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	TShaderMapRef< FGlobalComputeShader_Interface > weather_compute_shader_(GlobalShaderMap);


	weather_compute_shader_->Compute();



	return true;
}

void AWeatherManager::PrintResult()
{

}



