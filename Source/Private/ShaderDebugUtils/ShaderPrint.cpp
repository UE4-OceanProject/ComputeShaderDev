// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShaderPrint.h"
#include "ShaderPrintParameters.h"

#include "CommonRenderResources.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "Engine/Engine.h"
#include "GlobalShader.h"
#include "PipelineStateCache.h"
#include "RenderGraphBuilder.h"
#include "Renderer/Private/SceneRendering.h"
#include "Renderer/Private/SystemTextures.h"


//#include "SceneViewExtension.h"
//#include "RenderGraphResources.h"
//#include "RHIStaticStates.h"



namespace ShaderPrint_Custom
{
	// Console variables
	static TAutoConsoleVariable<int32> CVarEnable(
		TEXT("r.DebugShaderPrintEnable"),
		0,
		TEXT("DebugShaderPrint debugging toggle.\n"),
		ECVF_Cheat | ECVF_RenderThreadSafe);

	static TAutoConsoleVariable<int32> CVarFontSize(
		TEXT("r.DebugShaderPrintFontSize"),
		16,
		TEXT("DebugShaderPrint font size.\n"),
		ECVF_Cheat | ECVF_RenderThreadSafe);

	static TAutoConsoleVariable<int32> CVarFontSpacingX(
		TEXT("r.DebugShaderPrintFontSpacingX"),
		0,
		TEXT("DebugShaderPrint horizontal spacing between symbols.\n"),
		ECVF_Cheat | ECVF_RenderThreadSafe);

	static TAutoConsoleVariable<int32> CVarFontSpacingY(
		TEXT("r.DebugShaderPrintFontSpacingY"),
		8,
		TEXT("DebugShaderPrint vertical spacing between symbols.\n"),
		ECVF_Cheat | ECVF_RenderThreadSafe);

	static TAutoConsoleVariable<int32> CVarMaxValueCount(
		TEXT("r.DebugShaderPrintMaxValueCount"),
		2000,
		TEXT("DebugShaderPrint output buffer size.\n"),
		ECVF_Cheat | ECVF_RenderThreadSafe);


	// Structure used by shader buffers to store values and symbols
	struct ShaderPrintItem
	{
		FVector2D ScreenPos;
		int32 Value;
		int32 Type;
	};

	// Get value buffer size
	// Note that if the ShaderPrint system is disabled we still want to bind a minimal buffer
	int32 GetMaxValueCount()
	{
		int32 MaxValueCount = FMath::Max(CVarMaxValueCount.GetValueOnRenderThread(), 0);
		return IsEnabled() ? MaxValueCount : 0;
	}

	// Get symbol buffer size
	// This is some multiple of the value buffer size to allow for maximum value->symbol expansion
	int32 GetMaxSymbolCount()
	{
		return GetMaxValueCount() * 12;
	}

	// ShaderPrint uniform buffer
	//We dont need the below line because the default engine version already does this for us. Enable for when we convert to 3D debug (will need our own USF file)
	//For now we leave it because we can't compile without it
	IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FShaderPrintUniformBufferParameters, "ShaderPrintUniform");
	typedef TUniformBufferRef<FShaderPrintUniformBufferParameters> FUniformBufferRef;

	// Fill the uniform buffer parameters
	void SetUniformBufferParameters(FShaderPrintUniformBufferParameters& OutParameters)
	{
		FIntPoint SizeXY = GEngine->GameViewport->Viewport->GetSizeXY();

		const float FontWidth = (float)FMath::Max(CVarFontSize.GetValueOnRenderThread(), 1) / (float)FMath::Max(SizeXY.X, 1);
		const float FontHeight = (float)FMath::Max(CVarFontSize.GetValueOnRenderThread(), 1) / (float)FMath::Max(SizeXY.Y, 1);
		const float SpaceWidth = (float)FMath::Max(CVarFontSpacingX.GetValueOnRenderThread(), 1) / (float)FMath::Max(SizeXY.X, 1);
		const float SpaceHeight = (float)FMath::Max(CVarFontSpacingY.GetValueOnRenderThread(), 1) / (float)FMath::Max(SizeXY.Y, 1);

		OutParameters.FontSize = FVector4(FontWidth, FontHeight, SpaceWidth + FontWidth, SpaceHeight + FontHeight);

		OutParameters.MaxValueCount = GetMaxValueCount();
		OutParameters.MaxSymbolCount = GetMaxSymbolCount();
	}

	// Return a uniform buffer with values filled and with single frame lifetime
	FUniformBufferRef CreateUniformBuffer()
	{
		FShaderPrintUniformBufferParameters Parameters;
		SetUniformBufferParameters(Parameters);
		return FUniformBufferRef::CreateUniformBufferImmediate(Parameters, UniformBuffer_SingleFrame);
	}

	// Fill the FShaderParameters parameters
	void SetParameters(FShaderParameters& OutParameters)
	{
		OutParameters.UniformBufferParameters = CreateUniformBuffer();
		OutParameters.RWValuesBuffer = ShaderPrintValueBuffer.UAV;
	}



	template<typename TShaderRHIParamRef>


	// Supported platforms
	bool IsSupported(EShaderPlatform InShaderPlatform)
	{
		return RHISupportsComputeShaders(InShaderPlatform) && !IsHlslccShaderPlatform(InShaderPlatform);
	}

	bool IsSupported(FViewInfo const& View)
	{
		return true; // IsSupported(View.GetShaderPlatform());
	}

	bool IsEnabled()
	{
		return CVarEnable.GetValueOnAnyThread() != 0;
	}

	// Shader to initialize the output value buffer
	class FShaderInitValueBufferCS : public FGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(FShaderInitValueBufferCS);
		SHADER_USE_PARAMETER_STRUCT(FShaderInitValueBufferCS, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			SHADER_PARAMETER_UAV(RWStructuredBuffer<ShaderPrintItem>, RWValuesBuffer)
			END_SHADER_PARAMETER_STRUCT()

			static bool ShouldCompilePermutation(FGlobalShaderPermutationParameters const& Parameters)
		{
			return true; // IsSupported(Parameters.Platform);
		}
	};

	IMPLEMENT_GLOBAL_SHADER(FShaderInitValueBufferCS, "/Engine/Private/ShaderPrintDraw.usf", "InitValueBufferCS", SF_Compute);

	// Shader to fill the indirect parameter arguments ready for the value->symbol compute pass
	class FShaderBuildIndirectDispatchArgsCS : public FGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(FShaderBuildIndirectDispatchArgsCS);
		SHADER_USE_PARAMETER_STRUCT(FShaderBuildIndirectDispatchArgsCS, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			SHADER_PARAMETER_STRUCT_REF(FShaderPrintUniformBufferParameters, UniformBufferParameters)
			SHADER_PARAMETER_SRV(StructuredBuffer<ShaderPrintItem>, ValuesBuffer)
			SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<ShaderPrintItem>, RWSymbolsBuffer)
			SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint>, RWIndirectDispatchArgsBuffer)
			END_SHADER_PARAMETER_STRUCT()

			static bool ShouldCompilePermutation(FGlobalShaderPermutationParameters const& Parameters)
		{
			return true; // IsSupported(Parameters.Platform);
		}
	};

	IMPLEMENT_GLOBAL_SHADER(FShaderBuildIndirectDispatchArgsCS, "/Engine/Private/ShaderPrintDraw.usf", "BuildIndirectDispatchArgsCS", SF_Compute);

	// Shader to read the values buffer and convert to the symbols buffer
	class FShaderBuildSymbolBufferCS : public FGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(FShaderBuildSymbolBufferCS);
		SHADER_USE_PARAMETER_STRUCT(FShaderBuildSymbolBufferCS, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			SHADER_PARAMETER_STRUCT_REF(FShaderPrintUniformBufferParameters, UniformBufferParameters)
			SHADER_PARAMETER_SRV(StructuredBuffer<ShaderPrintItem>, ValuesBuffer)
			SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<ShaderPrintItem>, RWSymbolsBuffer)
			SHADER_PARAMETER_RDG_BUFFER(StructuredBuffer<uint>, IndirectDispatchArgsBuffer)
			END_SHADER_PARAMETER_STRUCT()

			static bool ShouldCompilePermutation(FGlobalShaderPermutationParameters const& Parameters)
		{
			return true; // IsSupported(Parameters.Platform);
		}
	};

	IMPLEMENT_GLOBAL_SHADER(FShaderBuildSymbolBufferCS, "/Engine/Private/ShaderPrintDraw.usf", "BuildSymbolBufferCS", SF_Compute);

	// Shader to fill the indirect parameter arguments ready for draw pass
	class FShaderBuildIndirectDrawArgsCS : public FGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(FShaderBuildIndirectDrawArgsCS);
		SHADER_USE_PARAMETER_STRUCT(FShaderBuildIndirectDrawArgsCS, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			SHADER_PARAMETER_STRUCT_REF(FShaderPrintUniformBufferParameters, UniformBufferParameters)
			SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<ShaderPrintItem>, SymbolsBuffer)
			SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint>, RWIndirectDrawArgsBuffer)
			END_SHADER_PARAMETER_STRUCT()

			static bool ShouldCompilePermutation(FGlobalShaderPermutationParameters const& Parameters)
		{
			return true; // IsSupported(Parameters.Platform);
		}
	};

	IMPLEMENT_GLOBAL_SHADER(FShaderBuildIndirectDrawArgsCS, "/Engine/Private/ShaderPrintDraw.usf", "BuildIndirectDrawArgsCS", SF_Compute);

	// Shader for draw pass to render each symbol
	class FShaderDrawSymbols : public FGlobalShader
	{
	public:
		SHADER_USE_PARAMETER_STRUCT(FShaderDrawSymbols, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			RENDER_TARGET_BINDING_SLOTS()
			SHADER_PARAMETER_STRUCT_REF(FShaderPrintUniformBufferParameters, UniformBufferParameters)
			SHADER_PARAMETER_TEXTURE(Texture2D, MiniFontTexture)
			SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<ShaderPrintItem>, SymbolsBuffer)
			SHADER_PARAMETER_RDG_BUFFER(StructuredBuffer<uint>, IndirectDrawArgsBuffer)
			END_SHADER_PARAMETER_STRUCT()

			static bool ShouldCompilePermutation(FGlobalShaderPermutationParameters const& Parameters)
		{
			return true; // IsSupported(Parameters.Platform);
		}
	};

	class FShaderDrawSymbolsVS : public FShaderDrawSymbols
	{
	public:
		DECLARE_GLOBAL_SHADER(FShaderDrawSymbolsVS);

		FShaderDrawSymbolsVS()
		{}

		FShaderDrawSymbolsVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
			: FShaderDrawSymbols(Initializer)
		{}
	};

	IMPLEMENT_GLOBAL_SHADER(FShaderDrawSymbolsVS, "/Engine/Private/ShaderPrintDraw.usf", "DrawSymbolsVS", SF_Vertex);

	class FShaderDrawSymbolsPS : public FShaderDrawSymbols
	{
	public:
		DECLARE_GLOBAL_SHADER(FShaderDrawSymbolsPS);

		FShaderDrawSymbolsPS()
		{}

		FShaderDrawSymbolsPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
			: FShaderDrawSymbols(Initializer)
		{}
	};

	IMPLEMENT_GLOBAL_SHADER(FShaderDrawSymbolsPS, "/Engine/Private/ShaderPrintDraw.usf", "DrawSymbolsPS", SF_Pixel);

	void BeginView(FRHICommandListImmediate& RHICmdList, FViewInfo& View)
	{
		if (!IsSupported(View))
		{
			return;
		}

		// Initialize output buffer and store in the view info
		// Values buffer contains Count + 1 elements. The first element is only used as a counter.
		ShaderPrintValueBuffer.Initialize(sizeof(ShaderPrintItem), GetMaxValueCount() + 1, 0U, TEXT("ShaderPrintValueBuffer"));

		// Early out if system is disabled
		// Note that we still prepared a minimal ShaderPrintValueBuffer 
		// This is in case some debug shader code is still active (we don't want an unbound buffer!)
		if (!IsEnabled())
		{
			return;
		}

		SCOPED_DRAW_EVENT(RHICmdList, ShaderPrintBeginView);

		// Clear the output buffer internal counter ready for use

			// Get the actual shader instance off the ShaderMap

		TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

		TShaderMapRef< FShaderInitValueBufferCS > ComputeShader(GlobalShaderMap);

		FShaderInitValueBufferCS::FParameters Parameters;
		Parameters.RWValuesBuffer = ShaderPrintValueBuffer.UAV;

		FComputeShaderUtils::Dispatch(RHICmdList, *ComputeShader, Parameters, FIntVector(1, 1, 1));
	}

	void DrawView(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef OutputTexture)
	{
		check(OutputTexture);

		RDG_EVENT_SCOPE(GraphBuilder, "ShaderPrintDrawView");

		// Initialize graph managed resources
		// Symbols buffer contains Count + 1 elements. The first element is only used as a counter.
		FRDGBufferRef SymbolBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(ShaderPrintItem), GetMaxSymbolCount() + 1), TEXT("ShaderPrintSymbolBuffer"));
		FRDGBufferRef IndirectDispatchArgsBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateIndirectDesc(4), TEXT("ShaderPrintIndirectDispatchArgs"));
		FRDGBufferRef IndirectDrawArgsBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateIndirectDesc(5), TEXT("ShaderPrintIndirectDrawArgs"));

		// Non graph managed resources
		FUniformBufferRef UniformBuffer = CreateUniformBuffer();
		FShaderResourceViewRHIRef ValuesBuffer = ShaderPrintValueBuffer.SRV;
		//FTextureRHIRef FontTexture = GEngine->MiniFontTexture != nullptr ? GEngine->MiniFontTexture->Resource->TextureRHI : GSystemTextures.BlackDummy->GetRenderTargetItem().ShaderResourceTexture;


		FTextureRHIRef FontTexture = GEngine->MiniFontTexture->Resource->TextureRHI;


		const ERHIFeatureLevel::Type FeatureLevel = View.GetFeatureLevel();
		TShaderMap<FGlobalShaderType>* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);

		// BuildIndirectDispatchArgs
		{
			typedef FShaderBuildIndirectDispatchArgsCS SHADER;
			TShaderMapRef<SHADER> ComputeShader(GlobalShaderMap);

			SHADER::FParameters* PassParameters = GraphBuilder.AllocParameters<SHADER::FParameters>();
			PassParameters->UniformBufferParameters = UniformBuffer;
			PassParameters->ValuesBuffer = ValuesBuffer;
			PassParameters->RWSymbolsBuffer = GraphBuilder.CreateUAV(SymbolBuffer, EPixelFormat::PF_R32_UINT);
			PassParameters->RWIndirectDispatchArgsBuffer = GraphBuilder.CreateUAV(IndirectDispatchArgsBuffer, EPixelFormat::PF_R32_UINT);

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("BuildIndirectDispatchArgs"),
				*ComputeShader, PassParameters,
				FIntVector(1, 1, 1));
		}

		// BuildSymbolBuffer
		{
			typedef FShaderBuildSymbolBufferCS SHADER;
			TShaderMapRef<SHADER> ComputeShader(GlobalShaderMap);

			SHADER::FParameters* PassParameters = GraphBuilder.AllocParameters<SHADER::FParameters>();
			PassParameters->UniformBufferParameters = UniformBuffer;
			PassParameters->ValuesBuffer = ValuesBuffer;
			PassParameters->RWSymbolsBuffer = GraphBuilder.CreateUAV(SymbolBuffer, EPixelFormat::PF_R32_UINT);
			PassParameters->IndirectDispatchArgsBuffer = IndirectDispatchArgsBuffer;

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("BuildSymbolBuffer"),
				*ComputeShader, PassParameters,
				IndirectDispatchArgsBuffer, 0);
		}

		// BuildIndirectDrawArgs
		{
			typedef FShaderBuildIndirectDrawArgsCS SHADER;
			TShaderMapRef<SHADER> ComputeShader(GlobalShaderMap);

			SHADER::FParameters* PassParameters = GraphBuilder.AllocParameters<SHADER::FParameters>();
			PassParameters->UniformBufferParameters = UniformBuffer;
			PassParameters->SymbolsBuffer = GraphBuilder.CreateSRV(SymbolBuffer);
			PassParameters->RWIndirectDrawArgsBuffer = GraphBuilder.CreateUAV(IndirectDrawArgsBuffer, EPixelFormat::PF_R32_UINT);

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("BuildIndirectDrawArgs"),
				*ComputeShader, PassParameters,
				FIntVector(1, 1, 1));
		}

		// DrawSymbols
		{
			typedef FShaderDrawSymbols SHADER;
			TShaderMapRef< FShaderDrawSymbolsVS > VertexShader(GlobalShaderMap);
			TShaderMapRef< FShaderDrawSymbolsPS > PixelShader(GlobalShaderMap);

			SHADER::FParameters* PassParameters = GraphBuilder.AllocParameters<SHADER::FParameters>();
			PassParameters->RenderTargets[0] = FRenderTargetBinding(OutputTexture, ERenderTargetLoadAction::ENoAction);
			PassParameters->UniformBufferParameters = UniformBuffer;
			PassParameters->MiniFontTexture = FontTexture;
			PassParameters->SymbolsBuffer = GraphBuilder.CreateSRV(SymbolBuffer);
			PassParameters->IndirectDrawArgsBuffer = IndirectDrawArgsBuffer;

			GraphBuilder.AddPass(
				RDG_EVENT_NAME("DrawSymbols"),
				PassParameters,
				ERDGPassFlags::Raster,
				[VertexShader, PixelShader, PassParameters](FRHICommandListImmediate& RHICmdListImmediate)
			{
				FGraphicsPipelineStateInitializer GraphicsPSOInit;
				RHICmdListImmediate.ApplyCachedRenderTargets(GraphicsPSOInit);
				GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
				GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_One, BF_InverseSourceAlpha, BO_Add, BF_Zero, BF_One>::GetRHI();
				GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
				GraphicsPSOInit.PrimitiveType = PT_TriangleList;
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();
				GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				SetGraphicsPipelineState(RHICmdListImmediate, GraphicsPSOInit);

				SetShaderParameters(RHICmdListImmediate, *VertexShader, VertexShader->GetVertexShader(), *PassParameters);
				SetShaderParameters(RHICmdListImmediate, *PixelShader, PixelShader->GetPixelShader(), *PassParameters);

				RHICmdListImmediate.DrawIndexedPrimitiveIndirect(GTwoTrianglesIndexBuffer.IndexBufferRHI, PassParameters->IndirectDrawArgsBuffer->GetIndirectRHICallBuffer(), 0);
			});
		}
	}

	void EndView(FViewInfo& View)
	{
		ShaderPrintValueBuffer.Release();
	}
}
