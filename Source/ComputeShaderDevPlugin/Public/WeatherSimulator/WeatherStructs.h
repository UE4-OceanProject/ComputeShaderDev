#pragma once
#include "WeatherStructs.generated.h"

//This needs to match the struct in the shader
USTRUCT(BlueprintType)
struct FStruct_Columns_CPU {
	GENERATED_USTRUCT_BODY()
		// Always make USTRUCT variables into UPROPERTY()
		// Any non-UPROPERTY() struct vars are not replicated
		// Always initialize your USTRUCT variables!

		///////////////////////////////////////////////////////
		// VariableData (5600)
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridVariables")
		TArray<float> Column;

	//Have to have this initializer or you will have "optimized out" issues when 
	//creating new structs?
	FStruct_Columns_CPU()
	{
	}
};


USTRUCT(BlueprintType)
struct FStruct_GridContainers_GPU {
	GENERATED_USTRUCT_BODY()
		// Always make USTRUCT variables into UPROPERTY()
		// Any non-UPROPERTY() struct vars are not replicated
		// Always initialize your USTRUCT variables!

		///////////////////////////////////////////////////////
		// INDEX TO VARIABLES (10)
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weather|GridVariables")
		TArray<FStruct_Columns_CPU> Grid;

	//Have to have this initializer or you will have "optimized out" issues when 
	//creating new structs?
	FStruct_GridContainers_GPU()
	{
	}
};