#include "Shader_Interface.h"
#include "ShaderParameterUtils.h" // Necessary for SetShaderValue, SetUniformBufferParameter.
//#include "RHIStaticStates.h"

//Here we "use" the named variable (struct) and link it to the name in the shader (.usf)

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(SlowUniformBuffer, "offset_yz");

//We already declared we using a shader in the .h file, now we link to WHICH shader .usf
//Declare our shader:   ShaderClassType						ShaderFileName													Shader function name		Type
IMPLEMENT_SHADER_TYPE(, FGlobalComputeShader_Interface, TEXT("/Plugin/ComputeShaderDev/Private/WeatherShader.usf"), TEXT("simulateStep"), SF_Compute);

//Shader interface: This tells ParameterMap which FShaderParameter and FShaderResourceParameters we want to bind
//Updates cached ParameterMap if we add a new param here
bool FGlobalComputeShader_Interface::Serialize(FArchive& Ar) {
	bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
	Ar
		<< dTParameter
		<< gridXParameter
		<< gridYParameter
		<< gridZParameter
		<< gridSizeIParameter
		<< gridSizeJParameter
		<< simulationTimeParameter
		<< prevGCParameter
		<< currGCParameter
		<< nextGCParameter

		<< FStruct_Cell_gridSizeK_CPU_ResourceParameter //single stack of values
		<< FStruct_GroundGridContainer_ground_CPU_ResourceParameter //100
		<< FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter //x5600
		<< FStruct_AirGridContainer_gridInit_CPU_ResourceParameter //x5600

		<< A_output_ //x5600x3
		<< B_output_ //x5600x3
		<< C_output_ //x5600x3
		;
	return bShaderHasOutdatedParameters;
}


FGlobalComputeShader_Interface::FGlobalComputeShader_Interface(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer) {
	//Here is where we take the "Serialized" map above, and "bind" each part to their respective variable name inside of the shader

	dTParameter.Bind(Initializer.ParameterMap, TEXT("dT"), SPF_Mandatory);
	gridXParameter.Bind(Initializer.ParameterMap, TEXT("gridX"), SPF_Mandatory);
	gridYParameter.Bind(Initializer.ParameterMap, TEXT("gridY"), SPF_Mandatory);
	gridZParameter.Bind(Initializer.ParameterMap, TEXT("gridZ"), SPF_Mandatory);
	gridSizeIParameter.Bind(Initializer.ParameterMap, TEXT("gridSizeI"), SPF_Mandatory);
	gridSizeJParameter.Bind(Initializer.ParameterMap, TEXT("gridSizeJ"), SPF_Mandatory);

	simulationTimeParameter.Bind(Initializer.ParameterMap, TEXT("simulationTime"), SPF_Mandatory);
	prevGCParameter.Bind(Initializer.ParameterMap, TEXT("prevGC"), SPF_Mandatory);
	currGCParameter.Bind(Initializer.ParameterMap, TEXT("currGC"), SPF_Mandatory);
	nextGCParameter.Bind(Initializer.ParameterMap, TEXT("nextGC"), SPF_Mandatory);

	FStruct_Cell_gridSizeK_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("gridSizeK"), SPF_Mandatory); //single stack of values
	FStruct_GroundGridContainer_ground_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("ground"), SPF_Mandatory); //100
	FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("gridRslow"), SPF_Mandatory); //x5600
	FStruct_AirGridContainer_gridInit_CPU_ResourceParameter.Bind(Initializer.ParameterMap, TEXT("gridInit"), SPF_Mandatory); //x5600


	////Frame loop
	//int a = 1;
	////First frame of simulation
	//output[a].Bind(Initializer.ParameterMap, TEXT("Current_Buffer"), SPF_Mandatory);//x5600x10
	//output[a+1].Bind(Initializer.ParameterMap, TEXT("Next_Buffer"), SPF_Mandatory);//x5600x10
	//output[a+2].Bind(Initializer.ParameterMap, TEXT("Future_Buffer"), SPF_Mandatory);//x5600x10

	////send to next frame

	////do some fancy math on indexes
	////just psuedo code
	//a = a + 1 % 3

	////and repeat loop


	A_output_.Bind(Initializer.ParameterMap, TEXT("test_outputA"), SPF_Mandatory);//x5600x3
	B_output_.Bind(Initializer.ParameterMap, TEXT("test_outputB"), SPF_Mandatory);//x5600x3
	C_output_.Bind(Initializer.ParameterMap, TEXT("test_outputC"), SPF_Mandatory);//x5600x3
}







//Here we set single FShaderParameter's
void FGlobalComputeShader_Interface::SetShaderParameters(FRHICommandList& RHICmdList, float dT,
	int gridX, int gridY, int gridZ, int gridSizeI, int gridSizeJ,
	float simulationTime,
	int prevGC, int currGC, int nextGC) {
	//SetShaderValue(RHICmdList, GetComputeShader(), offset_x_, offset_x);
	SetShaderValue(RHICmdList, GetComputeShader(), dTParameter, dT);
	SetShaderValue(RHICmdList, GetComputeShader(), gridXParameter, gridX);
	SetShaderValue(RHICmdList, GetComputeShader(), gridYParameter, gridY);
	SetShaderValue(RHICmdList, GetComputeShader(), gridZParameter, gridZ);
	SetShaderValue(RHICmdList, GetComputeShader(), gridSizeIParameter, gridSizeI);
	SetShaderValue(RHICmdList, GetComputeShader(), gridSizeJParameter, gridSizeJ);
	SetShaderValue(RHICmdList, GetComputeShader(), simulationTimeParameter, simulationTime);
	SetShaderValue(RHICmdList, GetComputeShader(), prevGCParameter, prevGC);
	SetShaderValue(RHICmdList, GetComputeShader(), currGCParameter, currGC);
	SetShaderValue(RHICmdList, GetComputeShader(), nextGCParameter, nextGC);
}

void FGlobalComputeShader_Interface::SetUniformBuffers(FRHICommandList& RHICmdList, const float offset_y, const float offset_z) {
	SlowUniformBuffer offset_yz;
	offset_yz.y = offset_y;
	offset_yz.z = offset_z;

	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<SlowUniformBuffer>(),
		TUniformBufferRef<SlowUniformBuffer>::CreateUniformBufferImmediate(offset_yz, UniformBuffer_MultiFrame));

	// NOTE: The last parameter: "EUniformBufferUsage Usage" has not been used in D3D11UniformBuffer.cpp.
	// NOTE: "r.UniformBufferPooling" is set to 1 (on) by default in ConsoleManager.cpp
	// UniformBuffer_SingleDraw: The uniform buffer is temporary, used for a single draw call then discarded
	// UniformBuffer_MultiFrame: The uniform buffer is used for multiple draw calls, possibly across multiple frames
}

void FGlobalComputeShader_Interface::SetShaderResourceParameters(FRHICommandList& RHICmdList
	//FShaderResourceViewRHIRef FStruct_Cell_gridSizeK_CPU_ResourceParameter_SRV,
	//FShaderResourceViewRHIRef FStruct_GroundGridContainer_ground_CPU_ResourceParameter_SRV,
	//FShaderResourceViewRHIRef FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_SRV,
	//FShaderResourceViewRHIRef FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_SRV
) {
	//if (FStruct_Cell_gridSizeK_CPU_ResourceParameter.IsBound()) {
	//	RHICmdList.SetShaderResourceViewParameter(GetComputeShader(), FStruct_Cell_gridSizeK_CPU_ResourceParameter.GetBaseIndex(), FStruct_Cell_gridSizeK_CPU_ResourceParameter_SRV);
	//}
	//if (FStruct_GroundGridContainer_ground_CPU_ResourceParameter.IsBound()) {
	//	RHICmdList.SetShaderResourceViewParameter(GetComputeShader(), FStruct_GroundGridContainer_ground_CPU_ResourceParameter.GetBaseIndex(), FStruct_GroundGridContainer_ground_CPU_ResourceParameter_SRV);
	//}
	//if (FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter.IsBound()) {
	//	RHICmdList.SetShaderResourceViewParameter(GetComputeShader(), FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter.GetBaseIndex(), FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_SRV);
	//}
	//if (FStruct_AirGridContainer_gridInit_CPU_ResourceParameter.IsBound()) {
	//	RHICmdList.SetShaderResourceViewParameter(GetComputeShader(), FStruct_AirGridContainer_gridInit_CPU_ResourceParameter.GetBaseIndex(), FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_SRV);
	//}
}


void FGlobalComputeShader_Interface::SetOutput(FRHICommandList& RHICmdList,
	FRHIUnorderedAccessView* A_output,
	FRHIUnorderedAccessView* B_output,
	FRHIUnorderedAccessView* C_output
) {
	buffer_index_rotator = ((buffer_index_rotator + 1) % 3);
	int buffer_index_rotator2 = ((buffer_index_rotator + 1) % 3);
	int buffer_index_rotator3 = ((buffer_index_rotator2 + 1) % 3);

	int a = buffer_index_rotator;
	int b = buffer_index_rotator2;
	int c = buffer_index_rotator3;

	TArray<FShaderResourceParameter>RotateableBufers = { {A_output_,B_output_,C_output_} };

	//RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[buffer_index_rotator].GetBaseIndex(), A_output);
	//RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[buffer_index_rotator + 1 % 3].GetBaseIndex(), B_output);
	//RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[buffer_index_rotator + 2 % 3].GetBaseIndex(), C_output);

	RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[a].GetBaseIndex(), A_output);
	RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[b].GetBaseIndex(), B_output);
	RHICmdList.SetUAVParameter(GetComputeShader(), RotateableBufers[c].GetBaseIndex(), C_output);

}


// for StructuredBuffer.
void FGlobalComputeShader_Interface::ClearParameters(FRHICommandList& RHICmdList) {

	RHICmdList.SetShaderResourceViewParameter(GetComputeShader(), FStruct_Cell_gridSizeK_CPU_ResourceParameter.GetBaseIndex(), FShaderResourceViewRHIRef());
	RHICmdList.SetShaderResourceViewParameter(GetComputeShader(), FStruct_GroundGridContainer_ground_CPU_ResourceParameter.GetBaseIndex(), FShaderResourceViewRHIRef());
	RHICmdList.SetShaderResourceViewParameter(GetComputeShader(), FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter.GetBaseIndex(), FShaderResourceViewRHIRef());
	RHICmdList.SetShaderResourceViewParameter(GetComputeShader(), FStruct_AirGridContainer_gridInit_CPU_ResourceParameter.GetBaseIndex(), FShaderResourceViewRHIRef());
}

// for RWStructuredBuffer.
void FGlobalComputeShader_Interface::ClearOutput(FRHICommandList& RHICmdList) {

	RHICmdList.SetUAVParameter(GetComputeShader(), A_output_.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	RHICmdList.SetUAVParameter(GetComputeShader(), B_output_.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	RHICmdList.SetUAVParameter(GetComputeShader(), C_output_.GetBaseIndex(), FUnorderedAccessViewRHIRef());
}


