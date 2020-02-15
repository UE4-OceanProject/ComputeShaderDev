#pragma once
#include "WeatherStructs.generated.h"

struct FWarpInConfig2
{
		float U; // u: wind component in the X direction
		float V; // v: wind component in the Y direction
		float W; // w: wind component in the Z direction
		float THETA; // Theta: Potential temperature
		float Pi; // Pi: Exener function
		float RO; // RO: Density (Temp)
		float QV; // qv: Vapor mixing ratio
		float QC; // qc: Condensation mixing ratio
		float QR; // qr: Rain mixing ratio
		float VORT; // Vort: Vorticity
};


//This needs to match the struct in the shader

USTRUCT(BlueprintType)
struct FStruct_Cell_CPU {
	GENERATED_USTRUCT_BODY()
		// Always make USTRUCT variables into UPROPERTY()
		// Any non-UPROPERTY() struct vars are not replicated
		// Always initialize your USTRUCT variables!

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridCellVariable")
		float Cell; 

					//Have to have this initializer or you will have "optimized out" issues when 
					//creating new structs?
		FStruct_Cell_CPU()
	{
	}
};


USTRUCT(BlueprintType)
struct FStruct_AirCellColumns_CPU {
	GENERATED_USTRUCT_BODY()
		// Always make USTRUCT variables into UPROPERTY()
		// Any non-UPROPERTY() struct vars are not replicated
		// Always initialize your USTRUCT variables!

		///////////////////////////////////////////////////////
		// INDEX TO VARIABLES (10)
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float U; // u: wind component in the X direction
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float V; // v: wind component in the Y direction
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float W; // w: wind component in the Z direction
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float THETA; // Theta: Potential temperature
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float Pi; // Pi: Exener function
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float RO; // RO: Density (Temp)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float QV; // qv: Vapor mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float QC; // qc: Condensation mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float QR; // qr: Rain mixing ratio
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridAirVariables")
		float VORT; // Vort: Vorticity

	//Have to have this initializer or you will have "optimized out" issues when 
	//creating new structs?
	FStruct_AirCellColumns_CPU()
	{
	}
};


USTRUCT(BlueprintType)
struct FStruct_AirGridContainer_CPU {
	GENERATED_USTRUCT_BODY()
		// Always make USTRUCT variables into UPROPERTY()
		// Any non-UPROPERTY() struct vars are not replicated
		// Always initialize your USTRUCT variables!

		///////////////////////////////////////////////////////
		// VariableData (5600)
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
		TArray<FStruct_AirCellColumns_CPU> CellColumns;

	//Have to have this initializer or you will have "optimized out" issues when 
	//creating new structs?
	FStruct_AirGridContainer_CPU()
	{
	}
};




USTRUCT(BlueprintType)
struct FStruct_GroundCellColumns_CPU {
	GENERATED_USTRUCT_BODY()
		// Always make USTRUCT variables into UPROPERTY()
		// Any non-UPROPERTY() struct vars are not replicated
		// Always initialize your USTRUCT variables!

		///////////////////////////////////////////////////////
		// INDEX TO VARIABLES (10)
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_TG; // Tg: Temperature of ground of first ds centimeters
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_TA; // Ta: Temperature of air above ground z>0
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_ALBEDO; // a: Albedo of the gridcell (heat absorbtion)
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_CGA; // Soild-heat capacity per area of the gridcell

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_TG_RESET; // Variables to reset after 24h
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_TA_RESET;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_TG_CORR;
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_TA_CORR;

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_BETA_INV; // Inverse of the Bowen ratio
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridGroundVariables")
			float GR_CLOUD_COVER; // Cloud coverage: Used in simulation and shadows

					//Have to have this initializer or you will have "optimized out" issues when 
					//creating new structs?
	FStruct_GroundCellColumns_CPU()
	{
	}
};


USTRUCT(BlueprintType)
struct FStruct_GroundGridContainer_CPU {
	GENERATED_USTRUCT_BODY()
		// Always make USTRUCT variables into UPROPERTY()
		// Any non-UPROPERTY() struct vars are not replicated
		// Always initialize your USTRUCT variables!

		///////////////////////////////////////////////////////
		// VariableData (5600)
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridVariables")
		TArray<FStruct_GroundCellColumns_CPU> CellColumns;

	//Have to have this initializer or you will have "optimized out" issues when 
	//creating new structs?
	FStruct_GroundGridContainer_CPU()
	{
	}
};