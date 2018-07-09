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
	int gridSizeI = gridXSize;
	int gridSizeJ = gridYSize;

	// Run sumulation
	
		for (int k = 1; k < gridZ - 1; k++) {
			for (int j = 0; j < gridY; j++) {
				for (int i = 0; i < gridX; i++) {

					//W column has special case

					int U_ijk = preCalc_CIJK(U, i, j, k);//
					int V_ijk = preCalc_CIJK(V, i, j, k);//
					int W_ijk = preCalc_WIJK(i, j, k);//
					int PI_ijk = preCalc_CIJK(PI, i, j, k);//
					int THETA_ijk = preCalc_CIJK(THETA, i, j, k);//
					int RO_ijk = preCalc_CIJK(RO, i, j, k);//
					int QV_ijk = preCalc_CIJK(QV, i, j, k);//
					int QC_ijk = preCalc_CIJK(QC, i, j, k);//
					int QR_ijk = preCalc_CIJK(QR, i, j, k);//



					int U_ip1jk = preCalc_CIJK(U, i + 1, j, k);//
					int V_ip1jk = preCalc_CIJK(V, i + 1, j, k);//			
					int W_ip1jk = preCalc_WIJK(i + 1, j, k);//
					int THETA_ip1jk = preCalc_CIJK(THETA, i + 1, j, k);//				
					int PI_ip1jk = preCalc_CIJK(PI, i + 1, j, k);//				
					int QV_ip1jk = preCalc_CIJK(QV, i + 1, j, k);//
					int QC_ip1jk = preCalc_CIJK(QC, i + 1, j, k);//
					int QR_ip1jk = preCalc_CIJK(QR, i + 1, j, k);//

					int U_im1jk = preCalc_CIJK(U, i - 1, j, k);//	
					int V_im1jk = preCalc_CIJK(V, i - 1, j, k);//				
					int W_im1jk = preCalc_WIJK(i - 1, j, k);//
					int THETA_im1jk = preCalc_CIJK(THETA, i - 1, j, k);//
					int PI_im1jk = preCalc_CIJK(PI, i - 1, j, k);//			
					int QV_im1jk = preCalc_CIJK(QV, i - 1, j, k);//
					int QC_im1jk = preCalc_CIJK(QC, i - 1, j, k);//
					int QR_im1jk = preCalc_CIJK(QR, i - 1, j, k);//

					int U_ijp1k = preCalc_CIJK(U, i, j + 1, k);//
					int V_ijp1k = preCalc_CIJK(V, i, j + 1, k);//
					int W_ijp1k = preCalc_WIJK(i, j + 1, k);//
					int THETA_ijp1k = preCalc_CIJK(THETA, i, j + 1, k);//
					int PI_ijp1k = preCalc_CIJK(PI, i, j + 1, k);//
					int QV_ijp1k = preCalc_CIJK(QV, i, j + 1, k);//
					int QC_ijp1k = preCalc_CIJK(QC, i, j + 1, k);//
					int QR_ijp1k = preCalc_CIJK(QR, i, j + 1, k);//

					int U_ijm1k = preCalc_CIJK(U, i, j - 1, k);//
					int V_ijm1k = preCalc_CIJK(V, i, j - 1, k);//
					int W_ijm1k = preCalc_WIJK(i, j - 1, k);//
					int THETA_ijm1k = preCalc_CIJK(THETA, i, j - 1, k);//	
					int PI_ijm1k = preCalc_CIJK(PI, i, j - 1, k);//				
					int QV_ijm1k = preCalc_CIJK(QV, i, j - 1, k);//
					int QC_ijm1k = preCalc_CIJK(QC, i, j - 1, k);//
					int QR_ijm1k = preCalc_CIJK(QR, i, j - 1, k);//

					int U_ijkp1 = preCalc_CIJK(U, i, j, k + 1);//
					int V_ijkp1 = preCalc_CIJK(V, i, j, k + 1);//
					int W_ijkp1 = preCalc_WIJK(i, j, k + 1);//
					int THETA_ijkp1 = preCalc_CIJK(THETA, i, j, k + 1);//
					int PI_ijkp1 = preCalc_CIJK(PI, i, j, k + 1);//
					int RO_ijkp1 = preCalc_CIJK(RO, i, j, k + 1);//
					int QV_ijkp1 = preCalc_CIJK(QV, i, j, k + 1);//
					int QC_ijkp1 = preCalc_CIJK(QC, i, j, k + 1);//				
					int QR_ijkp1 = preCalc_CIJK(QR, i, j, k + 1);//

					int U_ijkm1 = preCalc_CIJK(U, i, j, k - 1);//
					int V_ijkm1 = preCalc_CIJK(V, i, j, k - 1);//
					int W_ijkm1 = preCalc_WIJK(i, j, k - 1);//
					int THETA_ijkm1 = preCalc_CIJK(THETA, i, j, k - 1);//
					int PI_ijkm1 = preCalc_CIJK(PI, i, j, k - 1);//
					int RO_ijkm1 = preCalc_CIJK(RO, i, j, k - 1);//
					int QV_ijkm1 = preCalc_CIJK(QV, i, j, k - 1);//
					int QC_ijkm1 = preCalc_CIJK(QC, i, j, k - 1);//
					int QR_ijkm1 = preCalc_CIJK(QR, i, j, k - 1);//

					int V_im1jp1k = preCalc_CIJK(V, i - 1, j + 1, k);

					int W_im1jkp1 = preCalc_WIJK(i - 1, j, k + 1);//

					int U_ip1jm1k = preCalc_CIJK(U, i + 1, j - 1, k);//

					int U_ip1jkm1 = preCalc_CIJK(U, i + 1, j, k - 1);//

					int V_ijp1km1 = preCalc_CIJK(V, i, j + 1, k - 1);//

					int W_ijm1kp1 = preCalc_WIJK(i, j - 1, k + 1);//




					float curr_RO_ijk = currGC[RO_ijk];
					float curr_U_ijk = currGC[U_ijk];
					float curr_V_ijk = currGC[V_ijk];
					float curr_W_ijk = currGC[W_ijk];
					float curr_THETA_ijk = currGC[THETA_ijk];
					float init_THETA_ijk = gridInit[THETA_ijk];
					float init_QV_ijk = gridInit[QV_ijk];
					float curr_QV_ijk = currGC[QV_ijk];
					float curr_QC_ijk = currGC[QC_ijk];
					float curr_QR_ijk = currGC[QR_ijk];
					float curr_PI_ijk = currGC[PI_ijk];
					float prev_U_ijk = prevGC[U_ijk];
					float prev_V_ijk = prevGC[V_ijk];
					float curr_RO_ijkp1 = currGC[RO_ijkp1];
					float curr_RO_ijkm1 = currGC[RO_ijkm1];
					float init_THETA_ijkm1 = gridInit[THETA_ijkm1];
					float curr_W_ijkp1 = currGC[W_ijkp1];
					float curr_U_ip1jk = currGC[U_ip1jk];
					float curr_V_ijplk = currGC[V_ijp1k];
					float curr_RO_ijk_p_ijkp1 = (curr_RO_ijkp1 + curr_RO_ijk);
					float curr_RO_ijk_p_ijkm1 = (curr_RO_ijk + curr_RO_ijkm1);
					float init_THETA_ijk_p_p61_t_init_QV_ijk = (init_THETA_ijk + 0.61f * init_QV_ijk);



					float powGridSizeI2 = FMath::Pow(gridSizeI, 2);
					float powGridSizeJ2 = FMath::Pow(gridSizeJ, 2);
					float powGridSizeK2 = FMath::Pow(gridSizeK[k], 2);


					float curr_RO_t_gridSizeKk = (curr_RO_ijk * gridSizeK[k]);

					int km1 = k - 1;
					if (km1 < 0) km1 = 0;

//					const float Kx = 500.0f; // diffusion coefficients
//					const float Kz = 100.0f;

					gridRslow[U_ijk] =
						-1.0f / gridSizeI * (powf(0.5f * (currGC[U_ip1jk] + currGC[U_ijk]), 2) - powf(0.5f * (currGC[U_ijk] + currGC[U_im1jk]), 2)) // -duu/dx

						- 1.0f / gridSizeJ * (
							0.5f * (currGC[U_ijp1k] + currGC[U_ijk]) * 0.5f * (currGC[V_ijp1k] + currGC[V_im1jp1k])
							- 0.5f * (currGC[U_ijk] + currGC[U_ijm1k]) * 0.5f * (currGC[V_ijk] + currGC[V_im1jk]))// -duv/dy

							- 1.0f / (currGC[RO_ijk] * gridSizeK[k]) * (
								0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * 0.5f * (currGC[W_ijkp1] + currGC[W_im1jkp1]) * 0.5f * (currGC[U_ijkp1] + currGC[U_ijk])
								- 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * 0.5f * (currGC[W_ijk] + currGC[W_im1jk]) * 0.5f * (currGC[U_ijkm1] + currGC[U_ijk])) // -dpuw/dz
							- 1.0f / gridSizeI * (cpd * (gridInit[THETA_ijk] + 0.61f * gridInit[QV_ijk]) * (currGC[PI_ijk] - currGC[PI_im1jk])) // -cpd*T*dP/dx
							+ Kx / powf(gridSizeI, 2) * (prevGC[U_ip1jk] - 2.0f * prevGC[U_ijk] + prevGC[U_im1jk])
							+ Ky / powf(gridSizeJ, 2) * (prevGC[U_ijp1k] - 2.0f * prevGC[U_ijk] + prevGC[U_ijm1k])
							+ Kz / powf(gridSizeK[k], 2) * ((prevGC[U_ijkp1] - gridInit[U_ijkp1]) - 2.0f * (prevGC[U_ijk] - gridInit[U_ijk]) + (prevGC[U_ijkm1] - gridInit[U_ijkm1])); // Diffusion (implicit)
					
					gridRslow[V_ijk] =
						-1.0f / gridSizeI * (
							0.5f * (currGC[V_ip1jk] + currGC[V_ijk]) * 0.5f * (currGC[U_ip1jk] + currGC[U_ip1jm1k])
							- 0.5f * (currGC[V_ijk] + currGC[V_im1jk]) * 0.5f * (currGC[U_ijm1k] + currGC[U_ijk]))// -dvu/dx 

						- 1.0f / gridSizeJ * (powf(0.5f * (currGC[V_ijp1k] + currGC[V_ijk]), 2) - powf(0.5f * (currGC[V_ijk] + currGC[V_ijm1k]), 2)) // -dvv/dy

						- 1.0f / (currGC[RO_ijk] * gridSizeK[k]) * (
							0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * 0.5f * (currGC[W_ijkp1] + currGC[W_ijm1kp1]) * 0.5f * (currGC[V_ijkp1] + currGC[V_ijk])
							- 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * 0.5f * (currGC[W_ijk] + currGC[W_ijm1k]) * 0.5f * (currGC[V_ijkm1] + currGC[V_ijk])) // -dpvw/dz
						- 1.0f / gridSizeJ * (cpd * (gridInit[THETA_ijk] + 0.61f * gridInit[QV_ijk]) * (currGC[PI_ijk] - currGC[PI_ijm1k])) // -cpd*T*dP/dx
						+ Kx / powf(gridSizeI, 2) * (prevGC[V_ip1jk] - 2.0f * prevGC[V_ijk] + prevGC[V_im1jk])
						+ Ky / powf(gridSizeJ, 2) * (prevGC[V_ijp1k] - 2.0f * prevGC[V_ijk] + prevGC[V_ijm1k])
						+ Kz / powf(gridSizeK[k], 2) * ((prevGC[V_ijkp1] - gridInit[V_ijkp1]) - 2.0f * (prevGC[V_ijk] - gridInit[V_ijk]) + (prevGC[V_ijkm1] - gridInit[V_ijkm1])); // Diffusion (implicit)

					gridRslow[W_ijk] =
						-1.0f / gridSizeI * (
							0.5f * (currGC[U_ip1jk] + currGC[U_ip1jkm1]) * 0.5f * (currGC[W_ip1jk] + currGC[W_ijk])
							- 0.5f * (currGC[U_ijk] + currGC[U_ijkm1]) * 0.5f * (currGC[W_ijk] + currGC[W_im1jk])) // -duw/dx

						- 1.0f / gridSizeJ * (
							0.5f * (currGC[V_ijp1k] + currGC[V_ijp1km1]) * 0.5f * (currGC[W_ijp1k] + currGC[W_ijk])
							- 0.5f * (currGC[V_ijk] + currGC[V_ijkm1]) * 0.5f * (currGC[W_ijk] + currGC[W_ijm1k])) // -duw/dx

						- 1.0f / (0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * 0.5f * (gridSizeK[k] + gridSizeK[km1])) *
						(currGC[RO_ijk] * powf(0.5f * (currGC[W_ijkp1] + currGC[W_ijk]), 2) - currGC[RO_ijkm1] * powf(0.5f * (currGC[W_ijk] + currGC[W_ijkm1]), 2)) // -dpww/dz
						- 1.0f / (0.5f * (gridSizeK[k] + gridSizeK[km1])) *
						(cpd * 0.5f * (gridInit[THETA_ijk] + 0.61f * gridInit[QV_ijk] + gridInit[THETA_ijkm1] + 0.61f * gridInit[QV_ijkm1])
							* (currGC[PI_ijk] - currGC[PI_ijkm1])) // -cpd* T*dP/dz
						+ g * 0.5f * (currGC[THETA_ijk] / gridInit[THETA_ijk] + currGC[THETA_ijkm1] / gridInit[THETA_ijkm1]
							+ 0.61f* (currGC[QV_ijk] + currGC[QV_ijkm1]) - (currGC[QC_ijk] + currGC[QC_ijkm1] + currGC[QR_ijk] + currGC[QR_ijkm1])) // B=g* T'/T
						+ Kx / powf(gridSizeI, 2) * (prevGC[W_ip1jk] - 2.0f * prevGC[W_ijk] + prevGC[W_im1jk]) // Diffusion (implicit)
						+ Ky / powf(gridSizeJ, 2) * (prevGC[W_ijp1k] - 2.0f * prevGC[W_ijk] + prevGC[W_ijm1k]) // Diffusion (implicit)
						+ Kz / powf(gridSizeK[k], 2) * (prevGC[W_ijkp1] - 2.0f * prevGC[W_ijk] + prevGC[W_ijkm1]); // d2w/dx2+d2w/dz2

					gridRslow[THETA_ijk] =
						-1.0f / gridSizeI * (currGC[U_ip1jk] * 0.5f * (currGC[THETA_ip1jk] + currGC[THETA_ijk])
							- currGC[U_ijk] * 0.5f * (currGC[THETA_ijk] + currGC[THETA_im1jk])) // -duT/dx

						- 1.0f / gridSizeJ * (currGC[V_ijp1k] * 0.5f * (currGC[THETA_ijp1k] + currGC[THETA_ijk])
							- currGC[V_ijk] * 0.5f * (currGC[THETA_ijk] + currGC[THETA_ijm1k])) // -dvT/dx NN

						- 1.0f / (currGC[RO_ijk] * gridSizeK[k]) * (
							0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * currGC[W_ijkp1] * 0.5f * (currGC[THETA_ijkp1] + currGC[THETA_ijk])
							- 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * currGC[W_ijk] * 0.5f * (currGC[THETA_ijk] + currGC[THETA_ijkm1])) // -dpwT/dz
						- 1.0f / (currGC[RO_ijk]) * 0.5f * (
							0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * currGC[W_ijkp1] * (gridInit[THETA_ijkp1] - gridInit[THETA_ijk]) / gridSizeK[k + 1]
							+ 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * currGC[W_ijk] * (gridInit[THETA_ijk] - gridInit[THETA_ijkm1]) / gridSizeK[k]) // -w/p*dpT/dz (mean state)
						+ Kx / powf(gridSizeI, 2) * (prevGC[THETA_ip1jk] - 2.0f * prevGC[THETA_ijk] + prevGC[THETA_im1jk]) // Diffusion (implicit)
						+ Ky / powf(gridSizeJ, 2) * (prevGC[THETA_ijp1k] - 2.0f * prevGC[THETA_ijk] + prevGC[THETA_ijm1k]) // Diffusion (implicit)
						+ Kz / powf(gridSizeK[k], 2) * (prevGC[THETA_ijkp1] - 2.0f * prevGC[THETA_ijk] + prevGC[THETA_ijkm1]); // d2T/dx2+d2T/dz2

					gridRslow[PI_ijk] =
						-1.0f * (powf(cmax, 2) / (currGC[RO_ijk] * cpd * powf(gridInit[THETA_ijk] + 0.61f * gridInit[QV_ijk], 2))) * (// multiplier -cs^2/(cpd* p*T^2)
							+(currGC[RO_ijk] * (gridInit[THETA_ijk] + 0.61f * gridInit[QV_ijk]) * (currGC[U_ip1jk] - currGC[U_ijk])) / gridSizeI // pTdu/dx

							+ (currGC[RO_ijk] * (gridInit[THETA_ijk] + 0.61f * gridInit[QV_ijk]) * (currGC[V_ijp1k] - currGC[V_ijk])) / gridSizeJ // pTdv/dx

							+ (0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * currGC[W_ijkp1] * 0.5f * (gridInit[THETA_ijkp1] + gridInit[THETA_ijk])
								- 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * currGC[W_ijk] * 0.5f * (gridInit[THETA_ijk] + gridInit[THETA_ijkm1])) / gridSizeK[k] // pTdw/dz
							)
						+ Kx / powf(gridSizeI, 2) * (prevGC[PI_ip1jk] - 2.0f * prevGC[PI_ijk] + prevGC[PI_im1jk]) // Diffusion (implicit)
						+ Ky / powf(gridSizeI, 2) * (prevGC[PI_ijp1k] - 2.0f * prevGC[PI_ijk] + prevGC[PI_ijm1k]) // Diffusion (implicit)
						+ Kz / powf(gridSizeK[k], 2) * (prevGC[PI_ijkp1] - 2.0f * prevGC[PI_ijk] + prevGC[PI_ijkm1]); // d2P/dx2+d2P/dz2

																													  // Moisture terms
					gridRslow[QV_ijk] =
						-1.0f / gridSizeI * (currGC[U_ip1jk] * 0.5f * (currGC[QV_ip1jk] + currGC[QV_ijk])
							- currGC[U_ijk] * 0.5f * (currGC[QV_ijk] + currGC[QV_im1jk])) // -duqv/dx

						- 1.0f / gridSizeJ * (currGC[V_ijp1k] * 0.5f * (currGC[QV_ijp1k] + currGC[QV_ijk])
							- currGC[V_ijk] * 0.5f * (currGC[QV_ijk] + currGC[QV_ijm1k])) // -dvqv/dy

						- 1.0f / (currGC[RO_ijk] * gridSizeK[k]) * (
							0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * currGC[W_ijkp1] * 0.5f * (currGC[QV_ijkp1] + currGC[QV_ijk])
							- 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * currGC[W_ijk] * 0.5f * (currGC[QV_ijk] + currGC[QV_ijkm1])) // -dpwqv/dz
						- 1.0f / (currGC[RO_ijk]) * 0.5f * (
							0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * currGC[W_ijkp1] * (gridInit[QV_ijkp1] - gridInit[QV_ijk]) / gridSizeK[k + 1]
							+ 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * currGC[W_ijk] * (gridInit[QV_ijk] - gridInit[QV_ijkm1]) / gridSizeK[k]) // -w/p*dpqv/dz (mean state)
						+ Kx / powf(gridSizeI, 2) * (prevGC[QV_ip1jk] - 2.0f * prevGC[QV_ijk] + prevGC[QV_im1jk]) // Diffusion (implicit)
						+ Ky / powf(gridSizeI, 2) * (prevGC[QV_ijp1k] - 2.0f * prevGC[QV_ijk] + prevGC[QV_ijm1k]) // Diffusion (implicit)
						+ Kz / powf(gridSizeK[k], 2) * (prevGC[QV_ijkp1] - 2.0f * prevGC[QV_ijk] + prevGC[QV_ijkm1]); // d2q/dx2+d2q/dz2

					gridRslow[QC_ijk] =
						-1.0f / gridSizeI * (currGC[U_ip1jk] * 0.5f * (currGC[QC_ip1jk] + currGC[QC_ijk])
							- currGC[U_ijk] * 0.5f * (currGC[QC_ijk] + currGC[QC_im1jk])) // -duqv/dx

						- 1.0f / gridSizeJ * (currGC[V_ijp1k] * 0.5f * (currGC[QC_ijp1k] + currGC[QC_ijk])
							- currGC[V_ijk] * 0.5f * (currGC[QC_ijk] + currGC[QC_ijm1k])) // -duqv/dx

						- 1.0f / (currGC[RO_ijk] * gridSizeK[k]) * (
							0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * currGC[W_ijkp1] * 0.5f * (currGC[QC_ijkp1] + currGC[QC_ijk])
							- 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * currGC[W_ijk] * 0.5f * (currGC[QC_ijk] + currGC[QC_ijkm1])) // -dpwqv/dz
						+ Kx / powf(gridSizeI, 2) * (prevGC[QC_ip1jk] - 2.0f * prevGC[QC_ijk] + prevGC[QC_im1jk]) // Diffusion (implicit)
						+ Ky / powf(gridSizeI, 2) * (prevGC[QC_ijp1k] - 2.0f * prevGC[QC_ijk] + prevGC[QC_ijm1k]) // Diffusion (implicit)
						+ Kz / powf(gridSizeK[k], 2) * (prevGC[QC_ijkp1] - 2.0f * prevGC[QC_ijk] + prevGC[QC_ijkm1]); // d2q/dx2+d2q/dz2

					gridRslow[QR_ijk] =
						-1.0f / gridSizeI * (currGC[U_ip1jk] * 0.5f * (currGC[QR_ip1jk] + currGC[QR_ijk])
							- currGC[U_ijk] * 0.5f * (currGC[QR_ijk] + currGC[QR_im1jk])) // -duqv/dx

						- 1.0f / gridSizeJ * (currGC[V_ijp1k] * 0.5f * (currGC[QR_ijp1k] + currGC[QR_ijk])
							- currGC[V_ijk] * 0.5f * (currGC[QR_ijk] + currGC[QR_ijm1k])) // -dvqv/dy

						- 1.0f / (currGC[RO_ijk] * gridSizeK[k]) * (
							0.5f * (currGC[RO_ijkp1] + currGC[RO_ijk]) * currGC[W_ijkp1] * 0.5f * (currGC[QR_ijkp1] + currGC[QR_ijk])
							- 0.5f * (currGC[RO_ijk] + currGC[RO_ijkm1]) * currGC[W_ijk] * 0.5f * (currGC[QR_ijk] + currGC[QR_ijkm1])) // -dpwqv/dz
						+ Kx / powf(gridSizeI, 2) * (prevGC[QR_ip1jk] - 2.0f * prevGC[QR_ijk] + prevGC[QR_im1jk]) // Diffusion (implicit)
						+ Ky / powf(gridSizeI, 2) * (prevGC[QR_ijp1k] - 2.0f * prevGC[QR_ijk] + prevGC[QR_ijm1k]) // Diffusion (implicit)
						+ Kz / powf(gridSizeK[k], 2) * (prevGC[QR_ijkp1] - 2.0f * prevGC[QR_ijk] + prevGC[QR_ijkm1]); // d2q/dx2+d2q/dz2

					gridRslow[RO_ijk] = 0.0f;
				}
			}
		}



		// STEP 2

		/* STEP2: Kelsner Microphicis */
		for (int k = 1; k < gridZ - 1; k++) {
			for (int j = 0; j < gridY; j++) {
				for (int i = 0; i < gridX; i++) {

					int ijk = preCalcIJK(i, j, k);
					int ij0 = preCalcIJK(i, j, 0);
					int ijkp1 = preCalcIJK(i, j, k + 1);
					int ijkm1 = preCalcIJK(i, j, k - 1);
					float curr_RO_ijk = currGC[RO + ijk];
					float prev_p_grid_THETA = (prevGC[THETA + ijk] + gridInit[THETA + ijk]);

					// Kessler microphysics.
					// A = max[ k1 * (qc-qc0) , 0 ] : autoconverstion qc -> qr
					// B = k2 * qc * qr^7/8	: accretion qc -> qr
					// C: condensation ; qv <-> qv
					// E: evaporation ; qr -> qv
					// All values from t-1 step
					// Order of calculation matters here

					float A_conv = 0.0;
					if (prevGC[QC + ijk] > 0.001) A_conv = FMath::Max<float>(0.0f, 0.001* (prevGC[QC + ijk] - 0.001)); // conversion cloud -> rain

					float B_acc = FMath::Max<float>(0.0f, gridInit[RO + ijk] * 2.2f * prevGC[QC + ijk] * FMath::Pow(prevGC[QR + ijk], 0.875f)); // accretion cloud -> rain

					A_conv *= rainProbability;
					B_acc *= rainProbability;

					// Saturation adjustment (Soong & Ogura)
					float pmean = FMath::Pow(gridInit[Pi + ijk], cpd / Rd) *p_0; // Mean pressure
					float qvs = (380.0f / pmean) * exp(7.5f *log(10.0f) *
						(prev_p_grid_THETA * ((prevGC[Pi + ijk] + gridInit[Pi + ijk])) - 273.0f) /
						(prev_p_grid_THETA * ((prevGC[Pi + ijk] + gridInit[Pi + ijk])) - 36.0f)); // Saturation mixing ratio
					prevGC[QV + ijk] = FMath::Max<float>(prevGC[QV + ijk], -1.0f * gridInit[QV + ijk]); // remove negative values
					float rsub = qvs * (7.5f * logf(10.0f) * (273.0f - 36.0f) *Llv / cpd) /
						FMath::Pow(gridInit[Pi + ijk] * prev_p_grid_THETA, 2);

					float Cond = FMath::Min<float>(prevGC[QV + ijk] + gridInit[QV + ijk],
						FMath::Max<float>(0.0f, ((prevGC[QV + ijk] + gridInit[QV + ijk]) - qvs) / (1.0f + rsub))); // Condensation (qv -> qc)

					float Cvent = 1.6f + 124.9f * FMath::Pow(gridInit[RO + ijk] * prevGC[QC + ijk], 0.2046f); // ventillation factor
					float Evap = FMath::Min<float>(FMath::Min<float>(prevGC[QR + ijk], FMath::Max<float>(-1.0f *Cond - prevGC[QC + ijk], 0.0f)), // 3 options
						dT * Cvent * FMath::Pow(gridInit[RO + ijk] * prevGC[QR + ijk], 0.525f) / (5.4e5 + 2.55e8 / (pmean*qvs))
						*FMath::Max<float>(qvs - prevGC[QV + ijk], 0.0f) / (gridInit[RO + ijk] * qvs));
					Cond = FMath::Max<float>(Cond, -1.0f * prevGC[QC + ijk]);

					gridRslow[QV + ijk] = gridRslow[QV + ijk] - Cond + Evap; // Net mass conversion

					gridRslow[QC + ijk] = gridRslow[QC + ijk] + Cond - A_conv - B_acc; // Net mass conversion

					float vterm0 = 36.34f*sqrtf(gridInit[RO + ij0] / gridInit[RO + ijk]) * FMath::Pow(FMath::Max<float>(gridInit[RO + ijk] * prevGC[QR + ijk], 0.0f), 0.1364f);
					float vterm1 = 36.34f*sqrtf(gridInit[RO + ij0] / gridInit[RO + ijkp1]) * FMath::Pow(FMath::Max<float>(gridInit[RO + ijkp1] * prevGC[QR + ijkp1], 0.0f), 0.1364f); // vT terminal velocity
																																												// note, it's possible that vT > CFL.

					gridRslow[QR + ijk] = gridRslow[QR + ijk] + A_conv + B_acc - Evap // Net mass change
						+ 1.0f / (curr_RO_ijk * gridSizeK[k]) * (
							0.5f * (currGC[RO + ijkp1] + curr_RO_ijk) *vterm1* 0.5f * (prevGC[QR + ijkp1] + prevGC[QR + ijk])
							- 0.5f * (curr_RO_ijk + currGC[RO + ijkm1]) *vterm0* 0.5f * (prevGC[QR + ijk] + prevGC[QR + ijkm1])); // Falling rain

					gridRslow[THETA + ijk] = gridRslow[THETA + ijk] + Llv / (cpd * gridInit[Pi + ijk]) * (Cond - Evap); // latent heating Lv/(cpd * P) * (C-E);
				}
			}
		}
		// STEP 2

		/* STEP3: Move forward in time */
		for (int k = 1; k < gridZ - 1; k++) {
			for (int j = 0; j < gridY; j++) {
				for (int i = 0; i < gridX; i++) {

					int ijk = preCalcIJK(i, j, k);
					int Wijk = preCalc_WIJK(i, j, k);
					float curr_RO_ijk = currGC[RO + ijk];

					if (simulationTime == 0.0f) { // 1st step: forward in time
												  //	printf("first Iteration\n");
						nextGC[U + ijk] = currGC[U + ijk] + dT * gridRslow[U + ijk];
						nextGC[V + ijk] = currGC[V + ijk] + dT * gridRslow[V + ijk];
						nextGC[W + Wijk] = currGC[W + Wijk] + dT * gridRslow[W + Wijk];
						if ((k < 2)) nextGC[W + Wijk] = 0.0f; // top & bottom BCs //|| (k==zEnd)
						nextGC[Pi + ijk] = currGC[Pi + ijk] + dT * gridRslow[Pi + ijk];
						nextGC[THETA + ijk] = currGC[THETA + ijk] + dT * gridRslow[THETA + ijk];
						nextGC[QV + ijk] = currGC[QV + ijk] + dT * gridRslow[QV + ijk];
						nextGC[QC + ijk] = currGC[QC + ijk] + dT * gridRslow[QC + ijk];
						nextGC[QR + ijk] = currGC[QR + ijk] + dT * gridRslow[QR + ijk];
						nextGC[RO + ijk] = curr_RO_ijk + dT * gridRslow[RO + ijk];

					}
					else { // subsequent steps: leapfrog

						   //	printf("No first Iteration\n");
						nextGC[U + ijk] = prevGC[U + ijk] + 2.0f * dT * gridRslow[U + ijk];
						nextGC[V + ijk] = prevGC[V + ijk] + 2.0f * dT * gridRslow[V + ijk];
						nextGC[W + Wijk] = prevGC[W + Wijk] + 2.0f * dT * gridRslow[W + Wijk];
						if ((k < 2)) nextGC[W + Wijk] = 0.0f; // top & bottom BCs // || (k==zEnd)
						nextGC[Pi + ijk] = prevGC[Pi + ijk] + 2.0f * dT * gridRslow[Pi + ijk];
						nextGC[THETA + ijk] = prevGC[THETA + ijk] + 2.0f * dT * gridRslow[THETA + ijk];
						nextGC[QV + ijk] = prevGC[QV + ijk] + 2.0f * dT * gridRslow[QV + ijk];
						nextGC[QC + ijk] = prevGC[QC + ijk] + 2.0f * dT * gridRslow[QC + ijk];
						nextGC[QR + ijk] = prevGC[QR + ijk] + 2.0f * dT * gridRslow[QR + ijk];
						nextGC[RO + ijk] = prevGC[RO + ijk] + 2.0f * dT * gridRslow[RO + ijk];

						// Roberts-Asselin filter
						currGC[U + ijk] = currGC[U + ijk] + 0.1f * (nextGC[U + ijk] - 2.0f * currGC[U + ijk] + prevGC[U + ijk]);
						currGC[V + ijk] = currGC[V + ijk] + 0.1f * (nextGC[V + ijk] - 2.0f * currGC[V + ijk] + prevGC[V + ijk]);
						currGC[W + Wijk] = currGC[W + Wijk] + 0.1f * (nextGC[W + Wijk] - 2.0f * currGC[W + Wijk] + prevGC[W + Wijk]);
						currGC[THETA + ijk] = currGC[THETA + ijk] + 0.1f * (nextGC[THETA + ijk] - 2.0f * currGC[THETA + ijk] + prevGC[THETA + ijk]);
						currGC[Pi + ijk] = currGC[Pi + ijk] + 0.1f * (nextGC[Pi + ijk] - 2.0f * currGC[Pi + ijk] + prevGC[Pi + ijk]);
						currGC[QV + ijk] = currGC[QV + ijk] + 0.1f * (nextGC[QV + ijk] - 2.0f * currGC[QV + ijk] + prevGC[QV + ijk]);
						currGC[QC + ijk] = currGC[QC + ijk] + 0.1f * (nextGC[QC + ijk] - 2.0f * currGC[QC + ijk] + prevGC[QC + ijk]);
						currGC[QR + ijk] = currGC[QR + ijk] + 0.1f * (nextGC[QR + ijk] - 2.0f * currGC[QR + ijk] + prevGC[QR + ijk]);
						curr_RO_ijk = curr_RO_ijk + 0.1f * (nextGC[RO + ijk] - 2.0f * curr_RO_ijk + prevGC[RO + ijk]);
					}
				}
			}
		}
		// STEP 3

		/* STEP4: Radiation model */
		for (int j = 0; j < gridY; j++) {
			for (int i = 0; i < gridX; i++) {

				int ij0 = preCalcIJK(i, j, 0);
				int ij = preCalcIJ(i, j);

				const float T_M = 29.0f + 273.15f;// Invariable slab temperature //INIT 10.0f 32.0f
				const float dur = 3600.0f * 24.0f;// * 5.0f;//24h
				const float S_const = -1.127f;//Solar constant km/s

											  ////////////////////////////////////////
											  // INIT VALUES
				if (simulationTime == 0.0f) { // 1st step: forward in time 
					currGC[THETA + ij0] = 0;

					ground[GR_TG + ij] = 23.5f + 273.15f;
					ground[GR_TA + ij] = gridInit[THETA + ij0];

					ground[GR_TG_RESET + ij] = FLT_MAX;// INF

					ground[GR_TG_CORR + ij] = 0.0f;
					ground[GR_TA_CORR + ij] = 0.0f;

					ground[GR_CLOUD_COVER + ij] = 0.0f;
				}

				////////////////////////////////////////
				// CLOUD COVERAGE

				if ((int(simulationTime / dT) % (60 * 5)) == 0) {  // each 300 steps
					float cloudTotal = 0.0f;
					for (int z = static_cast<int>(0.33f * gridZ); z < static_cast<int>(0.83f * gridZ); z++) {
						int ijz = preCalcIJK(i, j, z);
						float density = nextGC[QC + ijz];
						if (density == 0.0f)
							continue;
						if (density > 2e-3f) {
							density = 0.99f;
						}
						else {
							if (density < 1e-3f) {
								density = 0.0f;
							}
							else {
								density = -1520000.0f * (density*density) + 5360.00f * (density)-3.74f;
							}
						}
						cloudTotal += density * 0.1f;  //0.05f so it has soft borders
						if (cloudTotal >= 1.0f) {
							break; // not necessary to check more
						}
					}
					ground[GR_CLOUD_COVER + ij] = fmin(cloudTotal, 1.0f);
				}


				////////////////////////////////////////
				// UTC
				float t_UTC = initTimeUTC_hours + (simulationTime / 3600.0f);//day overflow
				int advancedDays = int(t_UTC / (24.0f));//full days
				float  dayInYearUTC = initDayInYearUTC + advancedDays;
				while (dayInYearUTC > 365)
					dayInYearUTC -= 365;
				t_UTC -= advancedDays * 24.0f;

				////////////////////////////////////////
				// LOCAL
				float t_Local = t_UTC + timeZone;
				if (t_Local < 0.0f) t_Local += 24.0f;
				if (t_Local > 24.0f) t_Local -= 24.0f;


				float lat = latitudeRad;
				float longi = -longitudeRad;//note NEGATE (West)

				float delta = 0.409f*cos((2.0f * M_PI) * (dayInYearUTC - 173.0f) / (365.25f));//d_s: solarDeclineAngle
				float sinPSI = sin(lat) *sin(delta) - cos(lat) *cos(delta) *cos(((M_PI*t_UTC) / 12.0f) - longi);

				float gamma = 0.0000010717f * FMath::Pow(t_Local, 5.0f) + 0.0000818369f * FMath::Pow(t_Local, 4.0f) - 0.0060500842f * FMath::Pow(t_Local, 3.0f) + 0.0772306397f * FMath::Pow(t_Local, 2.0f) + 0.1444444444f*t_Local - 1.8441558442f;

				////////////////////////////////////////
				// RADITATION

				float alb = ground[GR_ALBEDO + ij];
				float c_g_a = ground[GR_CGA + ij];

				float sig_l = ground[GR_CLOUD_COVER + ij];
				float sig_m = ground[GR_CLOUD_COVER + ij];
				const float sig_h = 0.1f;

				float I = 0.08f* (1.0f - 0.1f *sig_h - 0.3f *sig_m - 0.6f *sig_l);

				float Tk = (0.6f + 0.2f *sinPSI) * (1.0f - 0.4f *sig_h) * (1.0f - 0.7f *sig_m) * (1.0f - 0.4f *sig_l);  //trans
				float Q_net = (1.0f - alb) * S_const * Tk *sinPSI + I;
				if (sinPSI < 0)
					Q_net = I;
				float a_fr;
				if (ground[GR_TG + ij] > ground[GR_TA + ij]) {
					//DAY
					a_fr = 3e-4f;
				}
				else {
					//NIGHT
					a_fr = 1.1e-4f;
				}

				float T_G_t = ((-Q_net / c_g_a) + (2.0f * M_PI / dur * (T_M - ground[GR_TG + ij])) - (a_fr* (ground[GR_TG + ij] - ground[GR_TA + ij])));
				float Q_g = -1.0f * ((c_g_a * T_G_t) + (2.0f * M_PI*c_g_a / dur * (ground[GR_TG + ij] - T_M)));  //Units are fomd
				float  Q_h = (-Q_net + Q_g) / ground[GR_BETA_INV + ij];

				ground[GR_TG + ij] += (dT *T_G_t) + ground[GR_TG_CORR + ij];// NEW TG
				ground[GR_TA + ij] += (dT * Q_h * 1.0e-3f) + ground[GR_TA_CORR + ij]; // Introduced this new time parameterization //NEW TA

				///////////////////////////////////
				// STEP 0: Save ref value after 2 hours of simulation
				if ((ground[GR_TG_RESET + ij] == FLT_MAX) && (simulationTime >= 3600.0f * 2.0f)) {  // put first FLT_MAX to avoid to comparisons
					ground[GR_TG_RESET + ij] = ground[GR_TG + ij];
					ground[GR_TA_RESET + ij] = ground[GR_TA + ij];
					if (i == 0 && j == 0) {
						printf("** Save Ref: %d (%.2f)\n", simulationTime, simulationTime / 3600.0f);
					}
				}

				///////////////////////////////////
				// STEP1: Update Correction after Each 24hours (+2h)
				if ((simulationTime >= 3600.0f * (2.0f + 24.0f)) && ((int(simulationTime) - 2 * 3600)) % (24 * 3600) == 0) { // RESET
					if (i == 0) {
						printf("** Reset: %d (%f)\n", simulationTime, simulationTime / 3600.0f);
					}
					float TG_diff = ground[GR_TG_RESET + ij] - ground[GR_TG + ij];
					float TA_diff = ground[GR_TA_RESET + ij] - ground[GR_TA + ij];
					ground[GR_TG_CORR + ij] = (TG_diff / (24.0f * 3600.0f)) * dT * 1.2f; //1.2f correction factor
					ground[GR_TA_CORR + ij] = (TA_diff / (24.0f * 3600.0f)) * dT * 1.2f;
				}

				nextGC[THETA + ij0] = ground[GR_TA + ij] + gamma * gridSizeK[0] / 100.0f - gridInit[THETA + ij0];//transfer of Ta to THETA
			}
		}
		//STEP 4


		simulationTime += dT;
	

}


