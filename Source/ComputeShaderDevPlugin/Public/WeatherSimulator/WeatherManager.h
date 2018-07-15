#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"
#include "RenderCommandFence.h"
#include "WeatherManager.generated.h"

//An actor based weather system for simulating weather
UCLASS(Blueprintable, BlueprintType)
class AWeatherManager : public AWeatherManager_Properties
{
	GENERATED_UCLASS_BODY()

public:

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//This is manually called in blueprints, everything else is automatically handled
	UFUNCTION(BlueprintCallable, Category = "ComputeShader")
		void Compute(float DeltaTime);





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
	void ExecuteComputeShader(TArray<FStruct_Columns_CPU> &currentStates, float DeltaTime);
	//This runs on the render thread
	void ExecuteInRenderThread(TArray<FStruct_Columns_CPU> &currentStates);

private:

	//Fencing forces the game thread to wait for the render thread to finish
	FRenderCommandFence ReleaseResourcesFence;
};