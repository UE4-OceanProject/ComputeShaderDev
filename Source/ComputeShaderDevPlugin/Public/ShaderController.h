// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GlobalComputeShader.h"
#include "RenderCommandFence.h"
#include "GameFramework/Actor.h"
#include "ShaderController.generated.h"

//This needs to match the struct in the shader
USTRUCT(BlueprintType)
struct FStruct_Shader_CPU {
	GENERATED_USTRUCT_BODY()
	// Always make USTRUCT variables into UPROPERTY()
	// Any non-UPROPERTY() struct vars are not replicated
	// Always initialize your USTRUCT variables!

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputeShader")
	int32 runCount = 1;
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
		TArray<FStruct_Shader_CPU> TArray_FStruct_Shader_CPU;





	//A structured buffer is just an array of data consisting of a single data type.
	//You can make a structured buffer of floats, or one of integers, but not one of floats and integers.

	//Global StructuredBuffer that is referenceable by another shader!
	//This is the output texture from the compute shader that we will pass to the pixel shader.
	//This is done when we set the reference (there are various flags that can be sent when setting the reference)
	//BUF_ShaderResource = Shareable between shaders
	//BUF_UnorderedAccess = Allows a UAV interface of this buffer to be created, which allows writting to this buffer

	FStructuredBufferRHIRef Interface_FStruct_Shader_GPU_Buffer;
	//Since the above parent buffer struct has the BUF_UnorderedAccess flag, we can use this as a writable buffer
	FUnorderedAccessViewRHIRef Interface_FStruct_Shader_GPU_Buffer_UAV;

	//You can do the same thing with a regular texture
	//FTexture2DRHIRef Texture;
	//FUnorderedAccessViewRHIRef TextureUAV;





protected:
	//This runs on the game thread
	void ExecuteComputeShader(TArray<FStruct_Shader_CPU> &currentStates, float DeltaTime);
	//This runs on the render thread
	void ExecuteInRenderThread(TArray<FStruct_Shader_CPU> &currentStates);

private:
	//These are the CPU accessable versions of our FStruct_Shader_GPU_Buffer structs that the shader uses
	//Note making changes are one way, any changes made to these inside of the 
	//shader hlsl code will not reflect on the CPU side
	FShaderConstants_Class Shader_Constant_Params;
	FVariables_Class Shader_Variable_Params;

	//Fencing forces the game thread to wait for the render thread to finish
	FRenderCommandFence ReleaseResourcesFence;
};
