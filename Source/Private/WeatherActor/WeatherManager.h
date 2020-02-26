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
			/* input */UPARAM(ref) TArray<FStruct_AirCellColumns_CPU>& input,
			/* output */TArray<FStruct_AirCellColumns_CPU>& output);


	///////////////////////////////////////
// WEATHER Constants

// Current Local Clock Time (LCT)




	UFUNCTION(BlueprintCallable, meta = (Keywords = "weather step"), Category = "Weather")
		void WeatherStep(UPARAM(ref) TArray<float>& prevGC, UPARAM(ref) TArray<float>& currGC, UPARAM(ref) TArray<float>& nextGC);
	UFUNCTION(BlueprintCallable, meta = (Keywords = "precalc xy"), Category = "Weather")
		int preCalcIJ(int x, int y);
	UFUNCTION(BlueprintCallable, meta = (Keywords = "precalc xyz"), Category = "Weather")
		int preCalcIJK(int x, int y, int z);
	UFUNCTION(BlueprintCallable, meta = (Keywords = "precalc wxyz"), Category = "Weather")
		int preCalc_WIJK(int x, int y, int z);
	UFUNCTION(BlueprintCallable, meta = (Keywords = "precalc wxyz"), Category = "Weather")
		int preCalc_CIJK(int c, int x, int y, int z);

	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step1"), Category = "Weather")
		void simulateSTEP1();
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step2"), Category = "Weather")
		void simulateSTEP2();
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step3"), Category = "Weather")
		void simulateSTEP3();
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step4"), Category = "Weather")
		void simulateSTEP4();

	int torid(int arr, int i, int j, int k);
	int torid_2(int x, int y);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



private:

};