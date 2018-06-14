// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "ObjectMacros.h"
#include "ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef COMPUTESHADERDEVPLUGIN_ComputeShaderFrontEnd_generated_h
#error "ComputeShaderFrontEnd.generated.h already included, missing '#pragma once' in ComputeShaderFrontEnd.h"
#endif
#define COMPUTESHADERDEVPLUGIN_ComputeShaderFrontEnd_generated_h

#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_12_GENERATED_BODY \
	friend COMPUTESHADERDEVPLUGIN_API class UScriptStruct* Z_Construct_UScriptStruct_FWeatherXYZPointState(); \
	COMPUTESHADERDEVPLUGIN_API static class UScriptStruct* StaticStruct();


#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execCompute) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_DeltaTime); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->Compute(Z_Param_DeltaTime); \
		P_NATIVE_END; \
	}


#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execCompute) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_DeltaTime); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->Compute(Z_Param_DeltaTime); \
		P_NATIVE_END; \
	}


#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAComputeShaderFrontEnd(); \
	friend COMPUTESHADERDEVPLUGIN_API class UClass* Z_Construct_UClass_AComputeShaderFrontEnd(); \
public: \
	DECLARE_CLASS(AComputeShaderFrontEnd, AActor, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/ComputeShaderDevPlugin"), NO_API) \
	DECLARE_SERIALIZER(AComputeShaderFrontEnd) \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_INCLASS \
private: \
	static void StaticRegisterNativesAComputeShaderFrontEnd(); \
	friend COMPUTESHADERDEVPLUGIN_API class UClass* Z_Construct_UClass_AComputeShaderFrontEnd(); \
public: \
	DECLARE_CLASS(AComputeShaderFrontEnd, AActor, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/ComputeShaderDevPlugin"), NO_API) \
	DECLARE_SERIALIZER(AComputeShaderFrontEnd) \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AComputeShaderFrontEnd(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AComputeShaderFrontEnd) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AComputeShaderFrontEnd); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AComputeShaderFrontEnd); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AComputeShaderFrontEnd(AComputeShaderFrontEnd&&); \
	NO_API AComputeShaderFrontEnd(const AComputeShaderFrontEnd&); \
public:


#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AComputeShaderFrontEnd(AComputeShaderFrontEnd&&); \
	NO_API AComputeShaderFrontEnd(const AComputeShaderFrontEnd&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AComputeShaderFrontEnd); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AComputeShaderFrontEnd); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AComputeShaderFrontEnd)


#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_PRIVATE_PROPERTY_OFFSET
#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_21_PROLOG
#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_PRIVATE_PROPERTY_OFFSET \
	ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_RPC_WRAPPERS \
	ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_INCLASS \
	ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_PRIVATE_PROPERTY_OFFSET \
	ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_RPC_WRAPPERS_NO_PURE_DECLS \
	ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_INCLASS_NO_PURE_DECLS \
	ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h_24_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID ComputeShaderDev_Plugins_ComputeShaderDevPlugin_Source_ComputeShaderDevPlugin_Public_ComputeShaderFrontEnd_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
