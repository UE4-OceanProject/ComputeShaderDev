/*=================================================
* FileName: WeatherManager.cpp
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
#include "WeatherSimulator/WeatherManager.h"
#include <algorithm> // for FMath::Min/max


AWeatherManager::AWeatherManager(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	//	PrimaryActorTick.bCanEverTick = true;
}

/* Operators to acces the data in a toroidal manner */
int AWeatherManager::preCalcIJK(int x, int y, int z)
{
	int test = 0;
	if (x == 9 && y == 9 && z == 55) {
		test = 0;
			
	};
	//Toroidal behavior: (x<0)-->(x=gridX-1-x) and (x>gridX-1)-->(x=x-gridX-1)
 	if (x < 0) x = gridX + x;
	else if (x >= gridX)x = (x%gridX);

	if (y < 0)y = gridY + y;
	else if (y >= gridY)y = (y%gridY);

	z = FMath::Max(0, z);//not under
	z = FMath::Min(z, gridZ - 2);//not over top
	
	// *10 = the amount of variables per cell
	return (x + y * gridX + z * (gridXY)*10);
}

int AWeatherManager::preCalc_CIJK(int c, int x, int y, int z)
{
	//Toroidal behavior: (x<0)-->(x=gridX-1-x) and (x>gridX-1)-->(x=x-gridX-1)
	if (x < 0) x = gridX + x;
	else if (x >= gridX)x = (x%gridX);

	if (y < 0)y = gridY + y;
	else if (y >= gridY)y = (y%gridY);

	z = FMath::Max(0, z);//not under
	z = FMath::Min(z, gridZ - 2);//not over top

	// *10 = the amount of variables per cell
	return (x + y * gridX + z * (gridXY) * 10) + c;
}

 /* Operators to acces the data in a toroidal manner */
int AWeatherManager::preCalc_WIJK(int x, int y, int z)
{
	//Toroidal behavior: (x<0)-->(x=gridX-1-x) and (x>gridX-1)-->(x=x-gridX-1)

	if (x < 0) x = gridX + x;
	else if (x >= gridX)x = (x%gridX);

	if (y < 0)y = gridY + y;
	else if (y >= gridY)y = (y%gridY);

	z = FMath::Max(0, z);//not under
	z = FMath::Min(z, gridZ - 1);//not over top

	return (x + y * gridX + z * (gridXY));
}//

 /* Operators to acces the data in a toroidal manner */
int AWeatherManager::preCalcIJ(int x, int y)
{
	int aY = y;  // toroidal behavior
	if (y < 0)aY = gridY + y;
	if (y >= gridY)aY = (y % gridY);

	int aX = x;  // toroidal behavior
	if (x < 0)aX = gridX + x;
	if (x >= gridX)aX = (x % gridX);
	// *10 = the amount of variables per cell
	return (aX + aY * gridY)*10;
}

void AWeatherManager::WeatherStep(UPARAM(ref) TArray<float>& prevGC, UPARAM(ref) TArray<float>& currGC, UPARAM(ref) TArray<float>& nextGC)
{

}


