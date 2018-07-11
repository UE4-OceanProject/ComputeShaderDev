/*=================================================
* FileName: WeatherSimulator.h
*
* Created by: SaschaElble
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2018/02/21
*
* Last Edited on:
* Last Edited by:
*
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#pragma once
#include "WeatherManager.generated.h"



//An actor based weather system for simulating weather
UCLASS(Blueprintable, BlueprintType)
class AWeatherManager : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	
	
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
		TArray<float> gridSizeK;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<float> gridSizeKAcc;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridX = int(side / gridXSize);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridY = int(side / gridYSize);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|PreInit")
		int gridZ = 56;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridXY = gridX * gridY;

	//not used in code, but used in bp to allocate array sizes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridXYZ = gridXY * gridZ; //Size of Weather array for 1 variable
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridXYx10 = gridXY * 10; //Size of Ground array with 10 variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|AutoInit")
		int gridXYZx10 = gridXYZ * 10; //Size of Weather array with 10 variables




	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<float> ground;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<float> gridInit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<float> gridRslow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<float> Grid3D0; //Past
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<float> Grid3D1; //Current
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|DataArrays")
		TArray<float> Grid3D2; //Future






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
		int RO = 5;		// RO: Density
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int QV = 6;		// qv: Vapor mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int QC = 7;		// qc: Condensation mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int QR = 8;		// qr: Rain mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirIndexing")
		int VORT = 9;	// Vort: Vorticity

						// INDEX TO GROUND VARIABLES
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_TG = 0;		// Tg: Temperature of ground of first ds centimeters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundIndexing")
		int GR_TA = 1;		// Ta: Temperature of air above grouind z>0
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


	

private:

};