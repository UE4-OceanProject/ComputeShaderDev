#pragma once
#include "WeatherManager.h"
#include "Shader_Interface.h"

#include "GenerateMips.h"
#include "RenderGraph.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "ShaderPrintParameters.h"


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

void AWeatherManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool AWeatherManager::Setup() 
{
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//SCOPED_DRAW_EVENT(RHICmdList, WeatherCalculate)


	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);





	ENQUEUE_RENDER_COMMAND(WeatherCompute)(
		[ComputeShader](FRHICommandListImmediate& RHICmdList)
	{
		ComputeShader->SetParameters(RHICmdList);
	});
	return true;
}

bool AWeatherManager::Calculate(
	/* input */ TArray<FStruct_AirGridContainer_CPU>& input,
	/* output */TArray<FStruct_AirGridContainer_CPU>& output)
{



	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//SCOPED_DRAW_EVENT(RHICmdList, WeatherCalculate)


	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);





	ENQUEUE_RENDER_COMMAND(WeatherCompute)(
	[ComputeShader](FRHICommandListImmediate& RHICmdList)
	{
		ComputeShader->Compute(RHICmdList);
	});
	

	return true;
}
