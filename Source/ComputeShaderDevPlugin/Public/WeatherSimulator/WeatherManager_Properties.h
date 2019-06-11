#pragma once
#include "GameFramework/Actor.h"
#include "WeatherStructs.h"
#include "WeatherManager_Properties.generated.h"

//An actor based weather system for simulating weather
UCLASS(Blueprintable, BlueprintType)
class AWeatherManager_Properties : public AActor
{
	GENERATED_UCLASS_BODY()

public:

	int prevGC = 0;
	int currGC = 0;
	int nextGC = 0;

	int numSteps = 1;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|SimulationVariables")
		int simulationTime = 0; // Current Local Clock Time (LCT)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|PreInit")
		int dT = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|PreInit")
		int side = 10000; // world will be side x side meters

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|PreInit")
		float gridXSize = 1000; // size of the grid
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|PreInit")
		float gridYSize = 1000; // size of the grid
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<FStruct_Cell_CPU> gridSizeK;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<float> gridSizeKAcc; // It is the size(in meters) of each gridcell accumulated in Z direction(Note: this contains the accumulated values, not the heights)

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridX = int(side / gridXSize);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridY = int(side / gridYSize);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|PreInit")
		int gridZ = 56;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridXY = gridX * gridY;


	// Radiation parameters (book values)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|RadiationParameters")
		float initDayInYearUTC = 100.0f; // out of 355.5f
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|RadiationParameters")
		float initTimeUTC_hours = 12.0f; // 0-24h
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|RadiationParameters")
		float timeZone = -6.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|RadiationParameters")
		float latitudeRad = 37.0f*DEG2RAD; // in radians 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|RadiationParameters")
		float longitudeRad = -122.0f*DEG2RAD; // in radians
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|RadiationParameters")
		float rainProbability = 1.0f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComputeShader")
	//	TArray<FStruct_AirGridContainer_CPU> TArray_FStruct_AirGridContainer_CPU_gridInit;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<FStruct_GroundGridContainer_CPU> ground;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<FStruct_AirGridContainer_CPU> gridInit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<FStruct_AirGridContainer_CPU> grid0Var;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<FStruct_AirGridContainer_CPU> gridRslow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<FStruct_AirGridContainer_CPU> Grid3D0; //Past
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
	//	TArray<FStruct_AirGridContainer_CPU> Grid3D1; //Current
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
	//	TArray<FStruct_AirGridContainer_CPU> Grid3D2; //Future



	// INDEX TO GROUND VARIABLES
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_TG = 0;		// Tg: Temperature of ground of first ds centimeters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_TA = 1;		// Ta: Temperature of air above ground z>0
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_ALBEDO = 2;	// a: Albedo of the gridcell (heat absorbtion)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_CGA = 3;		// Soild-heat capacity per area of the gridcell

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_TG_RESET = 4;	// Variables to reset after 24h
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_TA_RESET = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_TG_CORR = 6;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_TA_CORR = 7;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_BETA_INV = 8;	// Inverse of the Bowen ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_CLOUD_COVER = 9;	// Cloud coverage: Used in simulation and shadows

								///////////////////////////////////////////////////////
								// INDEX TO VARIABLES
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int U = 0;		// u: wind component in the X direction
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int V = 1;		// v: wind component in the Y direction
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int W = 2;		// w: wind component in the Z direction
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int THETA = 3;	// Theta: Potential temperature
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int Pi = 4;		// Pi: Exener function
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int RO = 5;		// RO: Density (Temp)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int QV = 6;		// qv: Vapor mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int QC = 7;		// qc: Condensation mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int QR = 8;		// qr: Rain mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int VORT = 9;	// Vort: Vorticity


						///////////////////////////////////////
						// WEATHER Constants

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float Kx = 500.; // diffusion coefficients
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float Ky = 500.; // diffusion coefficients
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float Kz = 100.;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float Aw = 1.0f; // is there vertical advection
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float Ab = 1.0f; // is there buoyancy

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float Rd = 287.05f; // J kg-1 K-1
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float epsilon = 0.622f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float p_0 = 100000.0f;// PA
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float T0 = 273.15f;	// K
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float cpd = 1004.5f;	// PA

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float cvd = 717.5f;	// J kg-1 K-1
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float Llv = 2.501e6;	// J kg-1
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float Rv = 461.5f;	// J kg-1 K-1
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float cpv = 1850.0f;	// J kg-1 K-1
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float cvv = 1390.0f;	// J kg-1 K-1
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float cpL = 4186.;	// J kg-1 K-1

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float es_T0 = 610.7f; // PA
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float g = 9.81f;		// m s-2
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float phi = 0.785398163f;  // 45 degree in radians

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float ro_0 = 1.225f;  // kg m-3
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float cmax = 50.0f;   // anelastic speed of sound (300ms-1)

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float M_PI = 3.1415927410125732421875f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|Constants")
		float DEG2RAD = M_PI / 180.0f;


private:

};