#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"
#include "RenderCommandFence.h"
#include "WeatherManager.generated.h"


//My Struct used to be here


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


protected:
	//This runs on the game thread
	void ExecuteComputeShader(TArray<FStruct_AirGridContainer_CPU> &currentStates, float DeltaTime);
	//This runs on the render thread
	void ExecuteInRenderThread(TArray<FStruct_AirGridContainer_CPU> &currentStates);

private:

	//Fencing forces the game thread to wait for the render thread to finish
	FRenderCommandFence ReleaseResourcesFence;
};