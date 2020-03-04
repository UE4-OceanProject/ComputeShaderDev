#pragma once
#include "WeatherManager_Properties.h"
#include "WeatherStructs.h"
#include "Shader_Interface.h"


#include "RenderGraph.h" //The only thing you need for RDG
#include "WeatherManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(WeatherManager, Verbose, All);

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
		bool Calculate();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step1"), Category = "Weather")
		void simulateSTEP1();
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step2"), Category = "Weather")
		void simulateSTEP2();
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step3"), Category = "Weather")
		void simulateSTEP3();
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step4"), Category = "Weather")
		void simulateSTEP4();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "Step4"), Category = "Weather")
		float GetDebugTotal();

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* MyMesh;


	int torid(int arr, int i, int j, int k);
	int torid_ground(int x, int y);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



private:

	float Step1TestTotal = 0;
	float Step2TestTotal = 0;
	float Step3CurrTestTotal = 0;
	float Step3NextTestTotal = 0;
	float Step4TestTotal = 0;

};
