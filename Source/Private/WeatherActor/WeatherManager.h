#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"


#include "RenderGraph.h" //The only thing you need for RDG
#include "WeatherManager.generated.h"


//An actor based weather system for simulating weather
UCLASS(Blueprintable, BlueprintType)
class AWeatherManager : public AWeatherManager_Properties
{
	GENERATED_UCLASS_BODY()

public:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Setup();

	UFUNCTION(BlueprintCallable, Category = "WeatherShader")
		bool Calculate(
			/* input */UPARAM(ref) TArray<FStruct_AirGridContainer_CPU>& input,
			/* output */TArray<FStruct_AirGridContainer_CPU>& output);


protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



private:

};