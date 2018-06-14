// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/ComputeShaderFrontEnd.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeComputeShaderFrontEnd() {}
// Cross Module References
	COMPUTESHADERDEVPLUGIN_API UScriptStruct* Z_Construct_UScriptStruct_FWeatherXYZPointState();
	UPackage* Z_Construct_UPackage__Script_ComputeShaderDevPlugin();
	COMPUTESHADERDEVPLUGIN_API UClass* Z_Construct_UClass_AComputeShaderFrontEnd_NoRegister();
	COMPUTESHADERDEVPLUGIN_API UClass* Z_Construct_UClass_AComputeShaderFrontEnd();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	COMPUTESHADERDEVPLUGIN_API UFunction* Z_Construct_UFunction_AComputeShaderFrontEnd_Compute();
// End Cross Module References
class UScriptStruct* FWeatherXYZPointState::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern COMPUTESHADERDEVPLUGIN_API uint32 Get_Z_Construct_UScriptStruct_FWeatherXYZPointState_CRC();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FWeatherXYZPointState, Z_Construct_UPackage__Script_ComputeShaderDevPlugin(), TEXT("WeatherXYZPointState"), sizeof(FWeatherXYZPointState), Get_Z_Construct_UScriptStruct_FWeatherXYZPointState_CRC());
	}
	return Singleton;
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FWeatherXYZPointState(FWeatherXYZPointState::StaticStruct, TEXT("/Script/ComputeShaderDevPlugin"), TEXT("WeatherXYZPointState"), false, nullptr, nullptr);
static struct FScriptStruct_ComputeShaderDevPlugin_StaticRegisterNativesFWeatherXYZPointState
{
	FScriptStruct_ComputeShaderDevPlugin_StaticRegisterNativesFWeatherXYZPointState()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("WeatherXYZPointState")),new UScriptStruct::TCppStructOps<FWeatherXYZPointState>);
	}
} ScriptStruct_ComputeShaderDevPlugin_StaticRegisterNativesFWeatherXYZPointState;
	UScriptStruct* Z_Construct_UScriptStruct_FWeatherXYZPointState()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FWeatherXYZPointState_CRC();
		UPackage* Outer = Z_Construct_UPackage__Script_ComputeShaderDevPlugin();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("WeatherXYZPointState"), sizeof(FWeatherXYZPointState), Get_Z_Construct_UScriptStruct_FWeatherXYZPointState_CRC(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
				{ "BlueprintType", "true" },
				{ "ModuleRelativePath", "Public/ComputeShaderFrontEnd.h" },
				{ "ToolTip", "This needs to match the struct in the shader" },
			};
#endif
			auto NewStructOpsLambda = []() -> void* { return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FWeatherXYZPointState>(); };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_instanceId_MetaData[] = {
				{ "Category", "ComputeShader" },
				{ "ModuleRelativePath", "Public/ComputeShaderFrontEnd.h" },
				{ "ToolTip", "Always make USTRUCT variables into UPROPERTY()\nAny non-UPROPERTY() struct vars are not replicated\nAlways initialize your USTRUCT variables!" },
			};
#endif
			static const UE4CodeGen_Private::FIntPropertyParams NewProp_instanceId = { UE4CodeGen_Private::EPropertyClass::Int, "instanceId", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(FWeatherXYZPointState, instanceId), METADATA_PARAMS(NewProp_instanceId_MetaData, ARRAY_COUNT(NewProp_instanceId_MetaData)) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_instanceId,
			};
			static const UE4CodeGen_Private::FStructParams ReturnStructParams = {
				(UObject* (*)())Z_Construct_UPackage__Script_ComputeShaderDevPlugin,
				nullptr,
				&UE4CodeGen_Private::TNewCppStructOpsWrapper<decltype(NewStructOpsLambda)>::NewCppStructOps,
				"WeatherXYZPointState",
				RF_Public|RF_Transient|RF_MarkAsNative,
				EStructFlags(0x00000001),
				sizeof(FWeatherXYZPointState),
				alignof(FWeatherXYZPointState),
				PropPointers, ARRAY_COUNT(PropPointers),
				METADATA_PARAMS(Struct_MetaDataParams, ARRAY_COUNT(Struct_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FWeatherXYZPointState_CRC() { return 166724694U; }
	void AComputeShaderFrontEnd::StaticRegisterNativesAComputeShaderFrontEnd()
	{
		UClass* Class = AComputeShaderFrontEnd::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "Compute", &AComputeShaderFrontEnd::execCompute },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, ARRAY_COUNT(Funcs));
	}
	UFunction* Z_Construct_UFunction_AComputeShaderFrontEnd_Compute()
	{
		struct ComputeShaderFrontEnd_eventCompute_Parms
		{
			float DeltaTime;
		};
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_DeltaTime = { UE4CodeGen_Private::EPropertyClass::Float, "DeltaTime", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000080, 1, nullptr, STRUCT_OFFSET(ComputeShaderFrontEnd_eventCompute_Parms, DeltaTime), METADATA_PARAMS(nullptr, 0) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_DeltaTime,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
				{ "Category", "ComputeShader" },
				{ "ModuleRelativePath", "Public/ComputeShaderFrontEnd.h" },
				{ "ToolTip", "This is manually called in blueprints, everything else is automatically handled" },
			};
#endif
			static const UE4CodeGen_Private::FFunctionParams FuncParams = { (UObject*(*)())Z_Construct_UClass_AComputeShaderFrontEnd, "Compute", RF_Public|RF_Transient|RF_MarkAsNative, nullptr, (EFunctionFlags)0x04020401, sizeof(ComputeShaderFrontEnd_eventCompute_Parms), PropPointers, ARRAY_COUNT(PropPointers), 0, 0, METADATA_PARAMS(Function_MetaDataParams, ARRAY_COUNT(Function_MetaDataParams)) };
			UE4CodeGen_Private::ConstructUFunction(ReturnFunction, FuncParams);
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_AComputeShaderFrontEnd_NoRegister()
	{
		return AComputeShaderFrontEnd::StaticClass();
	}
	UClass* Z_Construct_UClass_AComputeShaderFrontEnd()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_AActor,
				(UObject* (*)())Z_Construct_UPackage__Script_ComputeShaderDevPlugin,
			};
			static const FClassFunctionLinkInfo FuncInfo[] = {
				{ &Z_Construct_UFunction_AComputeShaderFrontEnd_Compute, "Compute" }, // 58336983
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "BlueprintType", "true" },
				{ "IncludePath", "ComputeShaderFrontEnd.h" },
				{ "ModuleRelativePath", "Public/ComputeShaderFrontEnd.h" },
			};
#endif
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_CS_WeatherPointStates_MetaData[] = {
				{ "Category", "ComputeShader" },
				{ "ModuleRelativePath", "Public/ComputeShaderFrontEnd.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_CS_WeatherPointStates = { UE4CodeGen_Private::EPropertyClass::Array, "CS_WeatherPointStates", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(AComputeShaderFrontEnd, CS_WeatherPointStates), METADATA_PARAMS(NewProp_CS_WeatherPointStates_MetaData, ARRAY_COUNT(NewProp_CS_WeatherPointStates_MetaData)) };
			static const UE4CodeGen_Private::FStructPropertyParams NewProp_CS_WeatherPointStates_Inner = { UE4CodeGen_Private::EPropertyClass::Struct, "CS_WeatherPointStates", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UScriptStruct_FWeatherXYZPointState, METADATA_PARAMS(nullptr, 0) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_CS_WeatherPointStates,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_CS_WeatherPointStates_Inner,
			};
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<AComputeShaderFrontEnd>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&AComputeShaderFrontEnd::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00800080u,
				FuncInfo, ARRAY_COUNT(FuncInfo),
				PropPointers, ARRAY_COUNT(PropPointers),
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AComputeShaderFrontEnd, 147170307);
	static FCompiledInDefer Z_CompiledInDefer_UClass_AComputeShaderFrontEnd(Z_Construct_UClass_AComputeShaderFrontEnd, &AComputeShaderFrontEnd::StaticClass, TEXT("/Script/ComputeShaderDevPlugin"), TEXT("AComputeShaderFrontEnd"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AComputeShaderFrontEnd);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
