#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"


#include "WeatherManager.generated.h"


//An actor based weather system for simulating weather
UCLASS(Blueprintable, BlueprintType)
class AWeatherManager : public AWeatherManager_Properties
{
	GENERATED_UCLASS_BODY()

public:
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Setup();

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Calculate(
			/*  input */const float x,
			/* input */UPARAM(ref) TArray<FStruct_AirGridContainer_CPU>& input,
			/* output */TArray<FStruct_AirGridContainer_CPU>& output);

protected:


private:
	int32 num_input_ = 2;
	//Store the number of rows in array for each array
	int32 gridSizeK_num_input_ = 0;
	int32 ground_num_input_ = 0;
	int32 gridRslow_num_input_ = 0;
	int32 gridInit_num_input_ = 0;
	int32 grid3D_num_input_ = 0;

	FVector offset_;


	void PrintResult();

};