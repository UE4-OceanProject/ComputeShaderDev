#pragma once
#include "WeatherManager.h"
#include "Shader_Interface.h"

#include "GenerateMips.h"
#include "RenderGraph.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "ShaderPrintParameters.h"


AWeatherManager::AWeatherManager(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWeatherManager::OnConstruction(const FTransform& Transform)
{
}

void AWeatherManager::BeginPlay()
{
	Super::BeginPlay();
}

void AWeatherManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {


	//C_output_buffer_.SafeRelease();
	//C_output_UAV_.SafeRelease();

	Super::EndPlay(EndPlayReason);
}

void AWeatherManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

bool AWeatherManager::Setup()
{
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//SCOPED_DRAW_EVENT(RHICmdList, WeatherCalculate)


	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);





	ENQUEUE_RENDER_COMMAND(WeatherCompute)(
		[ComputeShader](FRHICommandListImmediate& RHICmdList)
	{
		ComputeShader->SetParameters(RHICmdList);
	});
	return true;
}

bool AWeatherManager::Calculate(
	/* input */ TArray<FStruct_AirCellColumns_CPU>& input,
	/* output */TArray<FStruct_AirCellColumns_CPU>& output)
{



	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//SCOPED_DRAW_EVENT(RHICmdList, WeatherCalculate)


	TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef< FGlobalComputeShader_Interface > ComputeShader(GlobalShaderMap);





	ENQUEUE_RENDER_COMMAND(WeatherCompute)(
		[ComputeShader](FRHICommandListImmediate& RHICmdList)
	{
		ComputeShader->Compute(RHICmdList);
	});


	return true;
}


/* Operators to acces the data in a toroidal manner */
int AWeatherManager::preCalcIJK(int x, int y, int z)
{
	//Toroidal behavior: (x<0)-->(x=gridX-1-x) and (x>gridX-1)-->(x=x-gridX-1)
	if (x < 0) x = gridX + x;
	else if (x >= gridX)x = (x%gridX);

	if (y < 0)y = gridY + y;
	else if (y >= gridY)y = (y%gridY);

	z = FMath::Max(0, z);//not under
	z = FMath::Min(z, gridZ - 2);//not over top

	return (x + y * gridX + z * (gridXY) * 10);
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
	return (aX + aY * gridY) * 10;
}

void AWeatherManager::WeatherStep(UPARAM(ref) TArray<float>& C_prevGC, UPARAM(ref) TArray<float>& C_currGC, UPARAM(ref) TArray<float>& C_nextGC)
{
	int gridSizeI = gridXSize;
	int gridSizeJ = gridYSize;


	///////////////////////////////////////
	// SIMULATION STEPS



	int nextGC = (currGC + 1) % 3;  //3 number of time array
	int prevGC = (currGC - 1);
	if (prevGC < 0) prevGC = 2;  // Set the last step.

	simulateSTEP1();
	simulateSTEP2();
	simulateSTEP3();
	simulateSTEP4();

	currGC = (currGC + 1) % 3;  // 3 number of time array

	simulationTime += dT;


}

int AWeatherManager::torid_2(int x, int y) {
	int aY = y;  // toroidal behavior
	if (y < 0)aY = gridY + y;
	if (y >= gridY)aY = (y % gridY);

	int aX = x;  // toroidal behavior
	if (x < 0)aX = gridX + x;
	if (x >= gridX)aX = (x % gridX);

	return (aX + aY * gridY);//return that value
}


int AWeatherManager::torid(int arr, int x, int y, int z)
{
	int aI = x;  // Toroidal behavior: (x<0)-->(x=gridX-1-x) and (x>gridX-1)-->(x=x-gridX-1)
	if (aI < 0) aI = gridX + x;
	if (aI >= gridX)aI = (x%gridX);
	int aJ = y;  // Toroidal
	if (aJ < 0)aJ = gridY + y;
	if (aJ >= gridY)aJ = (y%gridY);
	int aZ = z;
	int gridK = gridZ;
	if (arr == W)
		gridK++;
	if ((z < 0 || z >= gridK) && (arr <= 2 || arr == 8)) {//for values (under z<0, or higher than the gridK) AND they are arr<=2 (u,v,w) or arr==8 (rL)
		return 0;
	}
	aZ = FMath::Max(0, aZ);//not under
	aZ = FMath::Min(aZ, gridK - 2);//not over top
	return aI + aJ * gridX + aZ * (gridX*gridY);


}


/* STEP 1 Fundamental Equations */
void AWeatherManager::simulateSTEP1() {
	float TestTotal = 0;
	for (int k = 1; k < gridZ - 1; k++) {
		for (int j = 0; j < gridY; j++) {
			for (int i = 0; i < gridX; i++) {

				int km1 = k - 1;
				if (km1 < 0) km1 = 0;

				const float Kx = 500.0f; // diffusion coefficients
				const float Kz = 100.0f;

				//Grid3D0/1/2
				int gridSizeI = gridXSize;
				int gridSizeJ = gridYSize;


				gridRslow[torid(U, i, j, k)].U =
					-1.0f / gridSizeI * (powf(0.5f * (Grid3D1[(torid(U, i + 1, j, k))].U + Grid3D1[torid(U, i, j, k)].U), 2) - powf(0.5f * (Grid3D1[torid(U, i, j, k)].U + Grid3D1[torid(U, i - 1, j, k)].U), 2)) // -duu/dx

					- 1.0f / gridSizeJ * (
						0.5f * (Grid3D1[torid(U, i, j + 1, k)].U + Grid3D1[torid(U, i, j, k)].U) * 0.5f * (Grid3D1[torid(V, i, j + 1, k)].V + Grid3D1[torid(V, i - 1, j + 1, k)].V)
						- 0.5f * (Grid3D1[torid(U, i, j, k)].U + Grid3D1[torid(U, i, j - 1, k)].U) * 0.5f * (Grid3D1[torid(V, i, j, k)].V + Grid3D1[torid(V, i - 1, j, k)].V))// -duv/dy

					- 1.0f / (Grid3D1[torid(RO, i, j, k)].RO * gridSizeK[k]) * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * 0.5f * (Grid3D1[torid(W, i, j, k + 1)].W + Grid3D1[torid(W, i - 1, j, k + 1)].W) * 0.5f * (Grid3D1[torid(U, i, j, k + 1)].U + Grid3D1[torid(U, i, j, k)].U)
						- 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * 0.5f * (Grid3D1[torid(W, i, j, k)].W + Grid3D1[torid(W, i - 1, j, k)].W) * 0.5f * (Grid3D1[torid(U, i, j, k - 1)].U + Grid3D1[torid(U, i, j, k)].U)) // -dpuw/dz
					- 1.0f / gridSizeI * (cpd * (gridInit[torid(THETA, i, j, k)].THETA + 0.61f * gridInit[torid(QV, i, j, k)].QV) * (Grid3D1[torid(Pi, i, j, k)].Pi - Grid3D1[torid(Pi, i - 1, j, k)].Pi)) // -cpd*T*dP/dx
					+ Kx / powf(gridSizeI, 2) * (Grid3D0[torid(U, i + 1, j, k)].U - 2.0f * Grid3D0[torid(U, i, j, k)].U + Grid3D0[torid(U, i - 1, j, k)].U)
					+ Ky / powf(gridSizeJ, 2) * (Grid3D0[torid(U, i, j + 1, k)].U - 2.0f * Grid3D0[torid(U, i, j, k)].U + Grid3D0[torid(U, i, j - 1, k)].U)
					+ Kz / powf(gridSizeK[k], 2) * ((Grid3D0[torid(U, i, j, k + 1)].U - gridInit[torid(U, i, j, k + 1)].U) - 2.0f * (Grid3D0[torid(U, i, j, k)].U - gridInit[torid(U, i, j, k)].U) + (Grid3D0[torid(U, i, j, k - 1)].U - gridInit[torid(U, i, j, k - 1)].U)); // Diffusion (implicit)


				gridRslow[torid(V, i, j, k)].V =
					-1.0f / gridSizeI * (
						0.5f * (Grid3D1[torid(V, i + 1, j, k)].V + Grid3D1[torid(V, i, j, k)].V) * 0.5f * (Grid3D1[torid(U, i + 1, j, k)].U + Grid3D1[torid(U, i + 1, j - 1, k)].U)
						- 0.5f * (Grid3D1[torid(V, i, j, k)].V + Grid3D1[torid(V, i - 1, j, k)].V) * 0.5f * (Grid3D1[torid(U, i, j - 1, k)].U + Grid3D1[torid(U, i, j, k)].U))// -dvu/dx 

					- 1.0f / gridSizeJ * (powf(0.5f * (Grid3D1[torid(V, i, j + 1, k)].V + Grid3D1[torid(V, i, j, k)].V), 2) - powf(0.5f * (Grid3D1[torid(V, i, j, k)].V + Grid3D1[torid(V, i, j - 1, k)].V), 2)) // -dvv/dy

					- 1.0f / (Grid3D1[torid(RO, i, j, k)].RO * gridSizeK[k]) * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * 0.5f * (Grid3D1[torid(W, i, j, k + 1)].W + Grid3D1[torid(W, i, j - 1, k + 1)].W) * 0.5f * (Grid3D1[torid(V, i, j, k + 1)].V + Grid3D1[torid(V, i, j, k)].V)
						- 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * 0.5f * (Grid3D1[torid(W, i, j, k)].W + Grid3D1[torid(W, i, j - 1, k)].W) * 0.5f * (Grid3D1[torid(V, i, j, k - 1)].V + Grid3D1[torid(V, i, j, k)].V)) // -dpvw/dz
					- 1.0f / gridSizeJ * (cpd * (gridInit[torid(THETA, i, j, k)].THETA + 0.61f * gridInit[torid(QV, i, j, k)].QV) * (Grid3D1[torid(Pi, i, j, k)].Pi - Grid3D1[torid(Pi, i, j - 1, k)].Pi)) // -cpd*T*dP/dx
					+ Kx / powf(gridSizeI, 2) * (Grid3D0[torid(V, i + 1, j, k)].V - 2.0f * Grid3D0[torid(V, i, j, k)].V + Grid3D0[torid(V, i - 1, j, k)].V)
					+ Ky / powf(gridSizeJ, 2) * (Grid3D0[torid(V, i, j + 1, k)].V - 2.0f * Grid3D0[torid(V, i, j, k)].V + Grid3D0[torid(V, i, j - 1, k)].V)
					+ Kz / powf(gridSizeK[k], 2) * ((Grid3D0[torid(V, i, j, k + 1)].V - gridInit[torid(V, i, j, k + 1)].V) - 2.0f * (Grid3D0[torid(V, i, j, k)].V - gridInit[torid(V, i, j, k)].V) + (Grid3D0[torid(V, i, j, k - 1)].V - gridInit[torid(V, i, j, k - 1)].V)); // Diffusion (implicit)

				gridRslow[torid(W, i, j, k)].W =
					-1.0f / gridSizeI * (
						0.5f * (Grid3D1[torid(U, i + 1, j, k)].U + Grid3D1[torid(U, i + 1, j, k - 1)].U) * 0.5f * (Grid3D1[torid(W, i + 1, j, k)].W + Grid3D1[torid(W, i, j, k)].W)
						- 0.5f * (Grid3D1[torid(U, i, j, k)].U + Grid3D1[torid(U, i, j, k - 1)].U) * 0.5f * (Grid3D1[torid(W, i, j, k)].W + Grid3D1[torid(W, i - 1, j, k)].W)) // -duw/dx

					- 1.0f / gridSizeJ * (
						0.5f * (Grid3D1[torid(V, i, j + 1, k)].V + Grid3D1[torid(V, i, j + 1, k - 1)].V) * 0.5f * (Grid3D1[torid(W, i, j + 1, k)].W + Grid3D1[torid(W, i, j, k)].W)
						- 0.5f * (Grid3D1[torid(V, i, j, k)].V + Grid3D1[torid(V, i, j, k - 1)].V) * 0.5f * (Grid3D1[torid(W, i, j, k)].W + Grid3D1[torid(W, i, j - 1, k)].W)) // -duw/dx

					- 1.0f / (0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * 0.5f * (gridSizeK[k] + gridSizeK[km1])) *
					(Grid3D1[torid(RO, i, j, k)].RO * powf(0.5f * (Grid3D1[torid(W, i, j, k + 1)].W + Grid3D1[torid(W, i, j, k)].W), 2) - Grid3D1[torid(RO, i, j, k - 1)].RO * powf(0.5f * (Grid3D1[torid(W, i, j, k)].W + Grid3D1[torid(W, i, j, k - 1)].W), 2)) // -dpww/dz
					- 1.0f / (0.5f * (gridSizeK[k] + gridSizeK[km1])) *
					(cpd * 0.5f * (gridInit[torid(THETA, i, j, k)].THETA + 0.61f * gridInit[torid(QV, i, j, k)].QV + gridInit[torid(THETA, i, j, k - 1)].THETA + 0.61f * gridInit[torid(QV, i, j, k - 1)].QV)
						* (Grid3D1[torid(Pi, i, j, k)].Pi - Grid3D1[torid(Pi, i, j, k - 1)].Pi)) // -cpd* T*dP/dz
					+ g * 0.5f * (Grid3D1[torid(THETA, i, j, k)].THETA / gridInit[torid(THETA, i, j, k)].THETA + Grid3D1[torid(THETA, i, j, k - 1)].THETA / gridInit[torid(THETA, i, j, k - 1)].THETA
						+ 0.61f* (Grid3D1[torid(QV, i, j, k)].QV + Grid3D1[torid(QV, i, j, k - 1)].QV) - (Grid3D1[torid(QC, i, j, k)].QC + Grid3D1[torid(QC, i, j, k - 1)].QC + Grid3D1[torid(QR, i, j, k)].QR + Grid3D1[torid(QR, i, j, k - 1)].QR)) // B=g* T'/T
					+ Kx / powf(gridSizeI, 2) * (Grid3D0[torid(W, i + 1, j, k)].W - 2.0f * Grid3D0[torid(W, i, j, k)].W + Grid3D0[torid(W, i - 1, j, k)].W) // Diffusion (implicit)
					+ Ky / powf(gridSizeJ, 2) * (Grid3D0[torid(W, i, j + 1, k)].W - 2.0f * Grid3D0[torid(W, i, j, k)].W + Grid3D0[torid(W, i, j - 1, k)].W) // Diffusion (implicit)
					+ Kz / powf(gridSizeK[k], 2) * (Grid3D0[torid(W, i, j, k + 1)].W - 2.0f * Grid3D0[torid(W, i, j, k)].W + Grid3D0[torid(W, i, j, k - 1)].W); // d2w/dx2+d2w/dz2

				gridRslow[torid(THETA, i, j, k)].THETA =
					-1.0f / gridSizeI * (Grid3D1[torid(U, i + 1, j, k)].U * 0.5f * (Grid3D1[torid(THETA, i + 1, j, k)].THETA + Grid3D1[torid(THETA, i, j, k)].THETA)
						- Grid3D1[torid(U, i, j, k)].U * 0.5f * (Grid3D1[torid(THETA, i, j, k)].THETA + Grid3D1[torid(THETA, i - 1, j, k)].THETA)) // -duT/dx

					- 1.0f / gridSizeJ * (Grid3D1[torid(V, i, j + 1, k)].V * 0.5f * (Grid3D1[torid(THETA, i, j + 1, k)].THETA + Grid3D1[torid(THETA, i, j, k)].THETA)
						- Grid3D1[torid(V, i, j, k)].V * 0.5f * (Grid3D1[torid(THETA, i, j, k)].THETA + Grid3D1[torid(THETA, i, j - 1, k)].THETA)) // -dvT/dx NN

					- 1.0f / (Grid3D1[torid(RO, i, j, k)].RO * gridSizeK[k]) * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * Grid3D1[torid(W, i, j, k + 1)].W * 0.5f * (Grid3D1[torid(THETA, i, j, k + 1)].THETA + Grid3D1[torid(THETA, i, j, k)].THETA)
						- 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * Grid3D1[torid(W, i, j, k)].W * 0.5f * (Grid3D1[torid(THETA, i, j, k)].THETA + Grid3D1[torid(THETA, i, j, k - 1)].THETA)) // -dpwT/dz
					- 1.0f / (Grid3D1[torid(RO, i, j, k)].RO) * 0.5f * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * Grid3D1[torid(W, i, j, k + 1)].W * (gridInit[torid(THETA, i, j, k + 1)].THETA - gridInit[torid(THETA, i, j, k)].THETA) / gridSizeK[k + 1]
						+ 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * Grid3D1[torid(W, i, j, k)].W * (gridInit[torid(THETA, i, j, k)].THETA - gridInit[torid(THETA, i, j, k - 1)].THETA) / gridSizeK[k]) // -w/p*dpT/dz (mean state)
					+ Kx / powf(gridSizeI, 2) * (Grid3D0[torid(THETA, i + 1, j, k)].THETA - 2.0f * Grid3D0[torid(THETA, i, j, k)].THETA + Grid3D0[torid(THETA, i - 1, j, k)].THETA) // Diffusion (implicit)
					+ Ky / powf(gridSizeJ, 2) * (Grid3D0[torid(THETA, i, j + 1, k)].THETA - 2.0f * Grid3D0[torid(THETA, i, j, k)].THETA + Grid3D0[torid(THETA, i, j - 1, k)].THETA) // Diffusion (implicit)
					+ Kz / powf(gridSizeK[k], 2) * (Grid3D0[torid(THETA, i, j, k + 1)].THETA - 2.0f * Grid3D0[torid(THETA, i, j, k)].THETA + Grid3D0[torid(THETA, i, j, k - 1)].THETA); // d2T/dx2+d2T/dz2

				gridRslow[torid(Pi, i, j, k)].Pi =
					-1.0f * (powf(cmax, 2) / (Grid3D1[torid(RO, i, j, k)].RO * cpd * powf(gridInit[torid(THETA, i, j, k)].THETA + 0.61f * gridInit[torid(QV, i, j, k)].QV, 2))) * (// multiplier -cs^2/(cpd* p*T^2)
						+(Grid3D1[torid(RO, i, j, k)].RO * (gridInit[torid(THETA, i, j, k)].THETA + 0.61f * gridInit[torid(QV, i, j, k)].QV) * (Grid3D1[torid(U, i + 1, j, k)].U - Grid3D1[torid(U, i, j, k)].U)) / gridSizeI // pTdu/dx

						+ (Grid3D1[torid(RO, i, j, k)].RO * (gridInit[torid(THETA, i, j, k)].THETA + 0.61f * gridInit[torid(QV, i, j, k)].QV) * (Grid3D1[torid(V, i, j + 1, k)].V - Grid3D1[torid(V, i, j, k)].V)) / gridSizeJ // pTdv/dx

						+ (0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * Grid3D1[torid(W, i, j, k + 1)].W * 0.5f * (gridInit[torid(THETA, i, j, k + 1)].THETA + gridInit[torid(THETA, i, j, k)].THETA)
							- 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * Grid3D1[torid(W, i, j, k)].W * 0.5f * (gridInit[torid(THETA, i, j, k)].THETA + gridInit[torid(THETA, i, j, k - 1)].THETA)) / gridSizeK[k] // pTdw/dz
						)
					+ Kx / powf(gridSizeI, 2) * (Grid3D0[torid(Pi, i + 1, j, k)].Pi - 2.0f * Grid3D0[torid(Pi, i, j, k)].Pi + Grid3D0[torid(Pi, i - 1, j, k)].Pi) // Diffusion (implicit)
					+ Ky / powf(gridSizeI, 2) * (Grid3D0[torid(Pi, i, j + 1, k)].Pi - 2.0f * Grid3D0[torid(Pi, i, j, k)].Pi + Grid3D0[torid(Pi, i, j - 1, k)].Pi) // Diffusion (implicit)
					+ Kz / powf(gridSizeK[k], 2) * (Grid3D0[torid(Pi, i, j, k + 1)].Pi - 2.0f * Grid3D0[torid(Pi, i, j, k)].Pi + Grid3D0[torid(Pi, i, j, k - 1)].Pi); // d2P/dx2+d2P/dz2

				  // Moisture terms
				gridRslow[torid(QV, i, j, k)].QV =
					-1.0f / gridSizeI * (Grid3D1[torid(U, i + 1, j, k)].U * 0.5f * (Grid3D1[torid(QV, i + 1, j, k)].QV + Grid3D1[torid(QV, i, j, k)].QV)
						- Grid3D1[torid(U, i, j, k)].U * 0.5f * (Grid3D1[torid(QV, i, j, k)].QV + Grid3D1[torid(QV, i - 1, j, k)].QV)) // -duqv/dx

					- 1.0f / gridSizeJ * (Grid3D1[torid(V, i, j + 1, k)].V * 0.5f * (Grid3D1[torid(QV, i, j + 1, k)].QV + Grid3D1[torid(QV, i, j, k)].QV)
						- Grid3D1[torid(V, i, j, k)].V * 0.5f * (Grid3D1[torid(QV, i, j, k)].QV + Grid3D1[torid(QV, i, j - 1, k)].QV)) // -dvqv/dy

					- 1.0f / (Grid3D1[torid(RO, i, j, k)].RO * gridSizeK[k]) * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * Grid3D1[torid(W, i, j, k + 1)].W * 0.5f * (Grid3D1[torid(QV, i, j, k + 1)].QV + Grid3D1[torid(QV, i, j, k)].QV)
						- 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * Grid3D1[torid(W, i, j, k)].W * 0.5f * (Grid3D1[torid(QV, i, j, k)].QV + Grid3D1[torid(QV, i, j, k - 1)].QV)) // -dpwqv/dz
					- 1.0f / (Grid3D1[torid(RO, i, j, k)].RO) * 0.5f * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * Grid3D1[torid(W, i, j, k + 1)].W * (gridInit[torid(QV, i, j, k + 1)].QV - gridInit[torid(QV, i, j, k)].QV) / gridSizeK[k + 1]
						+ 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * Grid3D1[torid(W, i, j, k)].W * (gridInit[torid(QV, i, j, k)].QV - gridInit[torid(QV, i, j, k - 1)].QV) / gridSizeK[k]) // -w/p*dpqv/dz (mean state)
					+ Kx / powf(gridSizeI, 2) * (Grid3D0[torid(QV, i + 1, j, k)].QV - 2.0f * Grid3D0[torid(QV, i, j, k)].QV + Grid3D0[torid(QV, i - 1, j, k)].QV) // Diffusion (implicit)
					+ Ky / powf(gridSizeI, 2) * (Grid3D0[torid(QV, i, j + 1, k)].QV - 2.0f * Grid3D0[torid(QV, i, j, k)].QV + Grid3D0[torid(QV, i, j - 1, k)].QV) // Diffusion (implicit)
					+ Kz / powf(gridSizeK[k], 2) * (Grid3D0[torid(QV, i, j, k + 1)].QV - 2.0f * Grid3D0[torid(QV, i, j, k)].QV + Grid3D0[torid(QV, i, j, k - 1)].QV); // d2q/dx2+d2q/dz2

				gridRslow[torid(QC, i, j, k)].QC =
					-1.0f / gridSizeI * (Grid3D1[torid(U, i + 1, j, k)].U * 0.5f * (Grid3D1[torid(QC, i + 1, j, k)].QC + Grid3D1[torid(QC, i, j, k)].QC)
						- Grid3D1[torid(U, i, j, k)].U * 0.5f * (Grid3D1[torid(QC, i, j, k)].QC + Grid3D1[torid(QC, i - 1, j, k)].QC)) // -duqv/dx

					- 1.0f / gridSizeJ * (Grid3D1[torid(V, i, j + 1, k)].V * 0.5f * (Grid3D1[torid(QC, i, j + 1, k)].QC + Grid3D1[torid(QC, i, j, k)].QC)
						- Grid3D1[torid(V, i, j, k)].V * 0.5f * (Grid3D1[torid(QC, i, j, k)].QC + Grid3D1[torid(QC, i, j - 1, k)].QC)) // -duqv/dx

					- 1.0f / (Grid3D1[torid(RO, i, j, k)].RO * gridSizeK[k]) * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * Grid3D1[torid(W, i, j, k + 1)].W * 0.5f * (Grid3D1[torid(QC, i, j, k + 1)].QC + Grid3D1[torid(QC, i, j, k)].QC)
						- 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * Grid3D1[torid(W, i, j, k)].W * 0.5f * (Grid3D1[torid(QC, i, j, k)].QC + Grid3D1[torid(QC, i, j, k - 1)].QC)) // -dpwqv/dz
					+ Kx / powf(gridSizeI, 2) * (Grid3D0[torid(QC, i + 1, j, k)].QC - 2.0f * Grid3D0[torid(QC, i, j, k)].QC + Grid3D0[torid(QC, i - 1, j, k)].QC) // Diffusion (implicit)
					+ Ky / powf(gridSizeI, 2) * (Grid3D0[torid(QC, i, j + 1, k)].QC - 2.0f * Grid3D0[torid(QC, i, j, k)].QC + Grid3D0[torid(QC, i, j - 1, k)].QC) // Diffusion (implicit)
					+ Kz / powf(gridSizeK[k], 2) * (Grid3D0[torid(QC, i, j, k + 1)].QC - 2.0f * Grid3D0[torid(QC, i, j, k)].QC + Grid3D0[torid(QC, i, j, k - 1)].QC); // d2q/dx2+d2q/dz2

				gridRslow[torid(QR, i, j, k)].QR =
					-1.0f / gridSizeI * (Grid3D1[torid(U, i + 1, j, k)].U * 0.5f * (Grid3D1[torid(QR, i + 1, j, k)].QR + Grid3D1[torid(QR, i, j, k)].QR)
						- Grid3D1[torid(U, i, j, k)].U * 0.5f * (Grid3D1[torid(QR, i, j, k)].QR + Grid3D1[torid(QR, i - 1, j, k)].QR)) // -duqv/dx

					- 1.0f / gridSizeJ * (Grid3D1[torid(V, i, j + 1, k)].V * 0.5f * (Grid3D1[torid(QR, i, j + 1, k)].QR + Grid3D1[torid(QR, i, j, k)].QR)
						- Grid3D1[torid(V, i, j, k)].V * 0.5f * (Grid3D1[torid(QR, i, j, k)].QR + Grid3D1[torid(QR, i, j - 1, k)].QR)) // -dvqv/dy

					- 1.0f / (Grid3D1[torid(RO, i, j, k)].RO * gridSizeK[k]) * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) * Grid3D1[torid(W, i, j, k + 1)].W * 0.5f * (Grid3D1[torid(QR, i, j, k + 1)].QR + Grid3D1[torid(QR, i, j, k)].QR)
						- 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) * Grid3D1[torid(W, i, j, k)].W * 0.5f * (Grid3D1[torid(QR, i, j, k)].QR + Grid3D1[torid(QR, i, j, k - 1)].QR)) // -dpwqv/dz
					+ Kx / powf(gridSizeI, 2) * (Grid3D0[torid(QR, i + 1, j, k)].QR - 2.0f * Grid3D0[torid(QR, i, j, k)].QR + Grid3D0[torid(QR, i - 1, j, k)].QR) // Diffusion (implicit)
					+ Ky / powf(gridSizeI, 2) * (Grid3D0[torid(QR, i, j + 1, k)].QR - 2.0f * Grid3D0[torid(QR, i, j, k)].QR + Grid3D0[torid(QR, i, j - 1, k)].QR) // Diffusion (implicit)
					+ Kz / powf(gridSizeK[k], 2) * (Grid3D0[torid(QR, i, j, k + 1)].QR - 2.0f * Grid3D0[torid(QR, i, j, k)].QR + Grid3D0[torid(QR, i, j, k - 1)].QR); // d2q/dx2+d2q/dz2

				gridRslow[torid(RO, i, j, k)].RO = 0.0f;

				//for quick verification
				TestTotal += gridRslow[torid(U, i, j, k)].U + gridRslow[torid(V, i, j, k)].V + gridRslow[torid(W, i, j, k)].W
					+ gridRslow[torid(THETA, i, j, k)].THETA + gridRslow[torid(Pi, i, j, k)].Pi
					+ gridRslow[torid(QV, i, j, k)].QV + gridRslow[torid(QC, i, j, k)].QC + gridRslow[torid(QR, i, j, k)].QR
					+ gridRslow[torid(RO, i, j, k)].RO;

				if (TestTotal != 0) {
					printf("////////////////////////////\nStep1 Test Total %d\n", TestTotal);
				}
			}
		}
	}
}
/* STEP2: Kelsner Microphicis */
void AWeatherManager::simulateSTEP2() {
	for (int k = 1; k < gridZ - 1; k++) {
		for (int j = 0; j < gridY; j++) {
			for (int i = 0; i < gridX; i++) {

				// Kessler microphysics.
				// A = max[ k1 * (qc-qc0) , 0 ] : autoconverstion qc -> qr
				// B = k2 * qc * qr^7/8	: accretion qc -> qr
				// C: condensation ; qv <-> qv
				// E: evaporation ; qr -> qv
				// All values from t-1 step
				// Order of calculation matters here

				float A_conv = 0.0;
				if (Grid3D0[torid(QC, i, j, k)].QC > 0.001) A_conv = FMath::Max<float>(0.0f, 0.001* (Grid3D0[torid(QC, i, j, k)].QC - 0.001)); // conversion cloud -> rain

				float B_acc = FMath::Max<float>(0.0f, gridInit[torid(RO, i, j, k)].RO * 2.2f * Grid3D0[torid(QC, i, j, k)].QC * powf(Grid3D0[torid(QR, i, j, k)].QR, 0.875f)); // accretion cloud -> rain

				A_conv *= /*st.*/rainProbability;
				B_acc *= /*st.*/rainProbability;

				// Saturation adjustment (Soong & Ogura)
				float pmean = powf(gridInit[torid(Pi, i, j, k)].Pi, cpd / Rd) *p_0; // Mean pressure
				float qvs = (380.0f / pmean) * exp(7.5f *log(10.0f) *
					((Grid3D0[torid(THETA, i, j, k)].THETA + gridInit[torid(THETA, i, j, k)].THETA) * ((Grid3D0[torid(Pi, i, j, k)].Pi + gridInit[torid(Pi, i, j, k)].Pi)) - 273.0f) /
					((Grid3D0[torid(THETA, i, j, k)].THETA + gridInit[torid(THETA, i, j, k)].THETA) * ((Grid3D0[torid(Pi, i, j, k)].Pi + gridInit[torid(Pi, i, j, k)].Pi)) - 36.0f)); // Saturation mixing ratio
				Grid3D0[torid(QV, i, j, k)].QV = FMath::Max<float>(Grid3D0[torid(QV, i, j, k)].QV, -1.0f * gridInit[torid(QV, i, j, k)].QV); // remove negative values
				float rsub = qvs * (7.5f * logf(10.0f) * (273.0f - 36.0f) *Llv / cpd) /
					powf(gridInit[torid(Pi, i, j, k)].Pi * (Grid3D0[torid(THETA, i, j, k)].THETA + gridInit[torid(THETA, i, j, k)].THETA), 2);

				float Cond = FMath::Min<float>(Grid3D0[torid(QV, i, j, k)].QV + gridInit[torid(QV, i, j, k)].QV,
					FMath::Max<float>(0.0f, ((Grid3D0[torid(QV, i, j, k)].QV + gridInit[torid(QV, i, j, k)].QV) - qvs) / (1.0f + rsub))); // Condensation (qv -> qc)

				float Cvent = 1.6f + 124.9f * powf(gridInit[torid(RO, i, j, k)].RO * Grid3D0[torid(QC, i, j, k)].QC, 0.2046f); // ventillation factor
				float Evap = FMath::Min<float>(FMath::Min<float>(Grid3D0[torid(QR, i, j, k)].QR, FMath::Max<float>(-1.0f *Cond - Grid3D0[torid(QC, i, j, k)].QC, 0.0f)), // 3 options
					dT * Cvent * powf(gridInit[torid(RO, i, j, k)].RO * Grid3D0[torid(QR, i, j, k)].QR, 0.525f) / (5.4e5 + 2.55e8 / (pmean*qvs))
					*FMath::Max<float>(qvs - Grid3D0[torid(QV, i, j, k)].QV, 0.0f) / (gridInit[torid(RO, i, j, k)].RO *qvs));
				Cond = FMath::Max<float>(Cond, -1.0f * Grid3D0[torid(QC, i, j, k)].QC);

				gridRslow[torid(QV, i, j, k)].QV = gridRslow[torid(QV, i, j, k)].QV - Cond + Evap; // Net mass conversion

				gridRslow[torid(QC, i, j, k)].QC = gridRslow[torid(QC, i, j, k)].QC + Cond - A_conv - B_acc; // Net mass conversion

				float vterm0 = 36.34f*sqrtf(gridInit[torid(RO, i, j, 0)].RO / gridInit[torid(RO, i, j, k)].RO) * powf(FMath::Max<float>(gridInit[torid(RO, i, j, k)].RO * Grid3D0[torid(QR, i, j, k)].QR, 0.0f), 0.1364f);
				float vterm1 = 36.34f*sqrtf(gridInit[torid(RO, i, j, 0)].RO / gridInit[torid(RO, i, j, k + 1)].RO) * powf(FMath::Max<float>(gridInit[torid(RO, i, j, k + 1)].RO * Grid3D0[torid(QR, i, j, k + 1)].QR, 0.0f), 0.1364f); // vT terminal velocity
				// note, it's possible that vT > CFL.

				gridRslow[torid(QR, i, j, k)].QR = gridRslow[torid(QR, i, j, k)].QR + A_conv + B_acc - Evap // Net mass change
					+ 1.0f / (Grid3D1[torid(RO, i, j, k)].RO * gridSizeK[k]) * (
						0.5f * (Grid3D1[torid(RO, i, j, k + 1)].RO + Grid3D1[torid(RO, i, j, k)].RO) *vterm1* 0.5f * (Grid3D0[torid(QR, i, j, k + 1)].QR + Grid3D0[torid(QR, i, j, k)].QR)
						- 0.5f * (Grid3D1[torid(RO, i, j, k)].RO + Grid3D1[torid(RO, i, j, k - 1)].RO) *vterm0* 0.5f * (Grid3D0[torid(QR, i, j, k)].QR + Grid3D0[torid(QR, i, j, k - 1)].QR)); // Falling rain

				gridRslow[torid(THETA, i, j, k)].THETA = gridRslow[torid(THETA, i, j, k)].THETA + Llv / (cpd * gridInit[torid(Pi, i, j, k)].Pi) * (Cond - Evap); // latent heating Lv/(cpd * P) * (C-E);
			}
		}
	}
}
/* STEP3: Move forward in time */
void AWeatherManager::simulateSTEP3() {
	for (int k = 1; k < gridZ - 1; k++) {
		for (int j = 0; j < gridY; j++) {
			for (int i = 0; i < gridX; i++) {

				if (simulationTime == 0.0f) { // 1st step: forward in time
				  //	printf("first Iteration\n");
					Grid3D2[torid(U, i, j, k)].U = Grid3D1[torid(U, i, j, k)].U + dT * gridRslow[torid(U, i, j, k)].U;
					Grid3D2[torid(V, i, j, k)].V = Grid3D1[torid(V, i, j, k)].V + dT * gridRslow[torid(V, i, j, k)].V;
					Grid3D2[torid(W, i, j, k)].W = Grid3D1[torid(W, i, j, k)].W + dT * gridRslow[torid(W, i, j, k)].W;
					if ((k < 2)) Grid3D2[torid(W, i, j, k)].W = 0.0f; // top & bottom BCs //|| (k==zEnd)
					Grid3D2[torid(Pi, i, j, k)].Pi = Grid3D1[torid(Pi, i, j, k)].Pi + dT * gridRslow[torid(Pi, i, j, k)].Pi;
					Grid3D2[torid(THETA, i, j, k)].THETA = Grid3D1[torid(THETA, i, j, k)].THETA + dT * gridRslow[torid(THETA, i, j, k)].THETA;
					Grid3D2[torid(QV, i, j, k)].QV = Grid3D1[torid(QV, i, j, k)].QV + dT * gridRslow[torid(QV, i, j, k)].QV;
					Grid3D2[torid(QC, i, j, k)].QC = Grid3D1[torid(QC, i, j, k)].QC + dT * gridRslow[torid(QC, i, j, k)].QC;
					Grid3D2[torid(QR, i, j, k)].QR = Grid3D1[torid(QR, i, j, k)].QR + dT * gridRslow[torid(QR, i, j, k)].QR;
					Grid3D2[torid(RO, i, j, k)].RO = Grid3D1[torid(RO, i, j, k)].RO + dT * gridRslow[torid(RO, i, j, k)].RO;

				}
				else { // subsequent steps: leapfrog

			   //	printf("No first Iteration\n");
					Grid3D2[torid(U, i, j, k)].U = Grid3D0[torid(U, i, j, k)].U + 2.0f * dT * gridRslow[torid(U, i, j, k)].U;
					Grid3D2[torid(V, i, j, k)].V = Grid3D0[torid(V, i, j, k)].V + 2.0f * dT * gridRslow[torid(V, i, j, k)].V;
					Grid3D2[torid(W, i, j, k)].W = Grid3D0[torid(W, i, j, k)].W + 2.0f * dT * gridRslow[torid(W, i, j, k)].W;
					if ((k < 2)) Grid3D2[torid(W, i, j, k)].W = 0.0f; // top & bottom BCs // || (k==zEnd)
					Grid3D2[torid(Pi, i, j, k)].Pi = Grid3D0[torid(Pi, i, j, k)].Pi + 2.0f * dT * gridRslow[torid(Pi, i, j, k)].Pi;
					Grid3D2[torid(THETA, i, j, k)].THETA = Grid3D0[torid(THETA, i, j, k)].THETA + 2.0f * dT * gridRslow[torid(THETA, i, j, k)].THETA;
					Grid3D2[torid(QV, i, j, k)].QV = Grid3D0[torid(QV, i, j, k)].QV + 2.0f * dT * gridRslow[torid(QV, i, j, k)].QV;
					Grid3D2[torid(QC, i, j, k)].QC = Grid3D0[torid(QC, i, j, k)].QC + 2.0f * dT * gridRslow[torid(QC, i, j, k)].QC;
					Grid3D2[torid(QR, i, j, k)].QR = Grid3D0[torid(QR, i, j, k)].QR + 2.0f * dT * gridRslow[torid(QR, i, j, k)].QR;
					Grid3D2[torid(RO, i, j, k)].RO = Grid3D0[torid(RO, i, j, k)].RO + 2.0f * dT * gridRslow[torid(RO, i, j, k)].RO;

					// Roberts-Asselin filter
					Grid3D1[torid(U, i, j, k)].U = Grid3D1[torid(U, i, j, k)].U + 0.1f * (Grid3D2[torid(U, i, j, k)].U - 2.0f * Grid3D1[torid(U, i, j, k)].U + Grid3D0[torid(U, i, j, k)].U);
					Grid3D1[torid(V, i, j, k)].V = Grid3D1[torid(V, i, j, k)].V + 0.1f * (Grid3D2[torid(V, i, j, k)].V - 2.0f * Grid3D1[torid(V, i, j, k)].V + Grid3D0[torid(V, i, j, k)].V);
					Grid3D1[torid(W, i, j, k)].W = Grid3D1[torid(W, i, j, k)].W + 0.1f * (Grid3D2[torid(W, i, j, k)].W - 2.0f * Grid3D1[torid(W, i, j, k)].W + Grid3D0[torid(W, i, j, k)].W);
					Grid3D1[torid(THETA, i, j, k)].THETA = Grid3D1[torid(THETA, i, j, k)].THETA + 0.1f * (Grid3D2[torid(THETA, i, j, k)].THETA - 2.0f * Grid3D1[torid(THETA, i, j, k)].THETA + Grid3D0[torid(THETA, i, j, k)].THETA);
					Grid3D1[torid(Pi, i, j, k)].Pi = Grid3D1[torid(Pi, i, j, k)].Pi + 0.1f * (Grid3D2[torid(Pi, i, j, k)].Pi - 2.0f * Grid3D1[torid(Pi, i, j, k)].Pi + Grid3D0[torid(Pi, i, j, k)].Pi);
					Grid3D1[torid(QV, i, j, k)].QV = Grid3D1[torid(QV, i, j, k)].QV + 0.1f * (Grid3D2[torid(QV, i, j, k)].QV - 2.0f * Grid3D1[torid(QV, i, j, k)].QV + Grid3D0[torid(QV, i, j, k)].QV);
					Grid3D1[torid(QC, i, j, k)].QC = Grid3D1[torid(QC, i, j, k)].QC + 0.1f * (Grid3D2[torid(QC, i, j, k)].QC - 2.0f * Grid3D1[torid(QC, i, j, k)].QC + Grid3D0[torid(QC, i, j, k)].QC);
					Grid3D1[torid(QR, i, j, k)].QR = Grid3D1[torid(QR, i, j, k)].QR + 0.1f * (Grid3D2[torid(QR, i, j, k)].QR - 2.0f * Grid3D1[torid(QR, i, j, k)].QR + Grid3D0[torid(QR, i, j, k)].QR);
					Grid3D1[torid(RO, i, j, k)].RO = Grid3D1[torid(RO, i, j, k)].RO + 0.1f * (Grid3D2[torid(RO, i, j, k)].RO - 2.0f * Grid3D1[torid(RO, i, j, k)].RO + Grid3D0[torid(RO, i, j, k)].RO);
				}
			}
		}
	}
}
/* STEP4: Radiation model */
void AWeatherManager::simulateSTEP4() {
	for (int j = 0; j < gridY; j++) {
		for (int i = 0; i < gridX; i++) {

			const float T_M = 29.0f + 273.15f;// Invariable slab temperature //INIT 10.0f 32.0f
			const float dur = 3600.0f * 24.0f;// * 5.0f;//24h
			const float S_const = -1.127f;//Solar constant km/s

			////////////////////////////////////////
			// INIT VALUES
			if (simulationTime == 0.0f) { // 1st step: forward in time 
				Grid3D1[torid(THETA, i, j, 0)].THETA = 0;

				ground[torid_2(i, j)].GR_TG = 23.5f + 273.15f;
				ground[torid_2(i, j)].GR_TA = gridInit[torid(THETA, i, j, 0)].THETA;

				ground[torid_2(i, j)].GR_TG_RESET = FLT_MAX;// INF

				ground[torid_2(i, j)].GR_TG_CORR = 0.0f;
				ground[torid_2(i, j)].GR_TA_CORR = 0.0f;

				ground[torid_2(i, j)].GR_CLOUD_COVER = 0.0f;
			}

			////////////////////////////////////////
			// CLOUD COVERAGE

			if ((int(simulationTime / dT) % (60 * 5)) == 0) {  // each 300 steps
				float cloudTotal = 0.0f;
				for (int z = static_cast<int>(0.33f * gridZ); z < static_cast<int>(0.83f * gridZ); z++) {
					float density = Grid3D2[torid(QC, i, j, z)].QC;
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
				ground[torid_2(i, j)].GR_CLOUD_COVER = fmin(cloudTotal, 1.0f);
			}


			////////////////////////////////////////
			// UTC
			float t_UTC = /*st.*/initTimeUTC_hours + (simulationTime / 3600.0f);//day overflow
			int advancedDays = int(t_UTC / (24.0f));//full days
			float  dayInYearUTC = /*st.*/initDayInYearUTC + advancedDays;
			while (dayInYearUTC > 365)
				dayInYearUTC -= 365;
			t_UTC -= advancedDays * 24.0f;

			////////////////////////////////////////
			// LOCAL
			float t_Local = t_UTC + /*st.*/timeZone;
			if (t_Local < 0.0f) t_Local += 24.0f;
			if (t_Local > 24.0f) t_Local -= 24.0f;


			float lat = /*st.*/latitudeRad;
			float longi = -/*st.*/longitudeRad;//note NEGATE (West)

			float delta = 0.409f*cos((2.0f * M_PI) * (dayInYearUTC - 173.0f) / (365.25f));//d_s: solarDeclineAngle
			float sinPSI = sin(lat) *sin(delta) - cos(lat) *cos(delta) *cos(((M_PI*t_UTC) / 12.0f) - longi);

			float gamma = 0.0000010717f * powf(t_Local, 5.0f) + 0.0000818369f * powf(t_Local, 4.0f) - 0.0060500842f * powf(t_Local, 3.0f) + 0.0772306397f * powf(t_Local, 2.0f) + 0.1444444444f*t_Local - 1.8441558442f;

			////////////////////////////////////////
			// RADITATION

			float alb = ground[torid_2(i, j)].GR_ALBEDO;
			float c_g_a = ground[torid_2(i, j)].GR_CGA;

			float sig_l = ground[torid_2(i, j)].GR_CLOUD_COVER;
			float sig_m = ground[torid_2(i, j)].GR_CLOUD_COVER;
			const float sig_h = 0.1f;

			float I = 0.08f* (1.0f - 0.1f *sig_h - 0.3f *sig_m - 0.6f *sig_l);

			float Tk = (0.6f + 0.2f *sinPSI) * (1.0f - 0.4f *sig_h) * (1.0f - 0.7f *sig_m) * (1.0f - 0.4f *sig_l);  //trans
			float Q_net = (1.0f - alb) * S_const * Tk *sinPSI + I;
			if (sinPSI < 0)
				Q_net = I;
			float a_fr;
			if (ground[torid_2(i, j)].GR_TG > ground[torid_2(i, j)].GR_TA) {
				//DAY
				a_fr = 3e-4f;
			}
			else {
				//NIGHT
				a_fr = 1.1e-4f;
			}

			float T_G_t = ((-Q_net / c_g_a) + (2.0f * M_PI / dur * (T_M - ground[torid_2(i, j)].GR_TG)) - (a_fr* (ground[torid_2(i, j)].GR_TG - ground[torid_2(i, j)].GR_TA)));
			float Q_g = -1.0f * ((c_g_a * T_G_t) + (2.0f * M_PI*c_g_a / dur * (ground[torid_2(i, j)].GR_TG - T_M)));  //Units are fomd
			float  Q_h = (-Q_net + Q_g) / ground[torid_2(i, j)].GR_BETA_INV;

			ground[torid_2(i, j)].GR_TG += (dT *T_G_t) + ground[torid_2(i, j)].GR_TG_CORR;// NEW TG
			ground[torid_2(i, j)].GR_TA += (dT * Q_h * 1.0e-3f) + ground[torid_2(i, j)].GR_TA_CORR; // Introduced this new time parameterization //NEW TA

			///////////////////////////////////
			// STEP 0: Save ref value after 2 hours of simulation
			if ((ground[torid_2(i, j)].GR_TG_RESET == FLT_MAX) && (simulationTime >= 3600.0f * 2.0f)) {  // put first FLT_MAX to avoid to comparisons
				ground[torid_2(i, j)].GR_TG_RESET = ground[torid_2(i, j)].GR_TG;
				ground[torid_2(i, j)].GR_TA_RESET = ground[torid_2(i, j)].GR_TA;
				if (i == 0 && j == 0) {
					printf("** Save Ref: %f (%.2f)\n", simulationTime, simulationTime / 3600.0f);
				}
			}

			///////////////////////////////////
			// STEP1: Update Correction after Each 24hours (+2h)
			if ((simulationTime >= 3600.0f * (2.0f + 24.0f)) && ((int(simulationTime) - 2 * 3600)) % (24 * 3600) == 0) { // RESET
				if (i == 0) {
					printf("** Reset: %f (%f)\n", simulationTime, simulationTime / 3600.0f);
				}
				float TG_diff = ground[torid_2(i, j)].GR_TG_RESET - ground[torid_2(i, j)].GR_TG;
				float TA_diff = ground[torid_2(i, j)].GR_TA_RESET - ground[torid_2(i, j)].GR_TA;
				ground[torid_2(i, j)].GR_TG_CORR = (TG_diff / (24.0f * 3600.0f)) * dT * 1.2f; //1.2f correction factor
				ground[torid_2(i, j)].GR_TA_CORR = (TA_diff / (24.0f * 3600.0f)) * dT * 1.2f;
			}

			Grid3D2[torid(THETA, i, j, 0)].THETA = ground[torid_2(i, j)].GR_TA + gamma * gridSizeK[0] / 100.0f - gridInit[torid(THETA, i, j, 0)].THETA;//transfer of Ta to THETA
		}
	}
}

