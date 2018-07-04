// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GlobalComputeShader.h"
#include "RenderCommandFence.h"
#include "GameFramework/Actor.h"
#include "ShaderController.generated.h"

//This needs to match the struct in the shader
USTRUCT(BlueprintType)
struct FWeatherXYZPointState {
	GENERATED_USTRUCT_BODY()
	// Always make USTRUCT variables into UPROPERTY()
	// Any non-UPROPERTY() struct vars are not replicated
	// Always initialize your USTRUCT variables!

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputeShader")
	int32 instanceId = 1;
};

UCLASS(BlueprintType)
class AShaderController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShaderController();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//This is manually called in blueprints, everything else is automatically handled
	UFUNCTION(BlueprintCallable, Category = "ComputeShader")
		void Compute(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputeShader")
		TArray<FWeatherXYZPointState> CS_WeatherPointStates;

protected:
	//This runs on the game thread
	void ExecuteComputeShader(TArray<FWeatherXYZPointState> &currentStates, float DeltaTime);
	//This runs on the render thread
	void ExecuteInRenderThread(TArray<FWeatherXYZPointState> &currentStates);

private:
	//These are the CPU accessable versions of our buffer structs that the shader uses
	//Note making changes are one way, any changes made to these inside of the 
	//shader hlsl code will not reflect on the CPU side
	FConstantParameters CS_ConstantParameters;
	FVariableParameters CS_VariableParameters;

	//Fencing forces the game thread to wait for the render thread to finish
	FRenderCommandFence ReleaseResourcesFence;
};
