//Need to make global pch file, so we can have "modules" in our code
//in a way to mimick modules during prototyping.
//These will eventually become

//Takes a texture and each pixel into 4x 8bit variables
//User is responceable for converting
//simulate an R8G8B8A8_UINT
#include "CoreUObject.h"
#include "Engine.h"
#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

uint8 Decode8888(uint32 uint32_data)
{
	return
			((uint32_data & 0x000000FF)),
			((uint32_data & 0x0000FF00) >> 8),
			((uint32_data & 0x00FF0000) >> 16),
			((uint32_data & 0xFF000000) >> 24)
	;
}

UTexture* CastRenderTargetToTexture(UTextureRenderTarget2D* RenderTarget)
{
	return Cast<UTexture>(RenderTarget);
}

char* GetTextureDataPtrFromGpu_LOCKING(FTexture2DRHIRef Texture, FRHICommandListImmediate& RHICmdList)
{
	uint32 stride_uint32 = 0;
	return (char*)RHICmdList.LockTexture2D(Texture, 0, EResourceLockMode::RLM_ReadOnly, stride_uint32, false);
}

char* ReleaseTextureDataPtrFromGpu_UNLOCKING(FTexture2DRHIRef Texture, FRHICommandListImmediate& RHICmdList)
{
	uint32 stride_uint32 = 0;
	return (char*)RHICmdList.LockTexture2D(Texture, 0, EResourceLockMode::RLM_ReadOnly, stride_uint32, false);
}

TArray<FColor> uint32_TextureToBitMap(FTexture2DRHIRef Texture, FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());
	
	//To access our resource we do a custom read using lockrect
	uint32 stride_uint32 = 0;

	//create a bitmap
	TArray<FColor> Bitmap;

	char* TextureDataPtr = GetTextureDataPtrFromGpu_LOCKING(Texture, RHICmdList);

	for (uint32 Row = 0; Row < Texture->GetSizeY(); ++Row)
	{
		uint32* PixelPtr = (uint32*)TextureDataPtr;
	

		//Since we are using our custom UINT format, we need to unpack it here to access the actual colors
		for (uint32 Col = 0; Col < Texture->GetSizeX(); ++Col)
		{
			uint32 Encoded32 = *PixelPtr;
			Bitmap.Add(FColor(Decode8888(Encoded32)));

			PixelPtr++;
		}

		// move to next row:
		TextureDataPtr += stride_uint32;
	}
	ReleaseTextureDataPtrFromGpu_UNLOCKING(Texture, RHICmdList);
	return Bitmap;
}

void SaveBitmap_bmp(TArray<FColor> Bitmap, uint32 TextureSizeY)//Texture->GetSizeY()
{
	// if the format and texture type is supported
	if (Bitmap.Num())
	{
		// Create screenshot folder if not already present.
		IFileManager::Get().MakeDirectory(*FPaths::ScreenShotDir(), true);

		const FString ScreenFileName(FPaths::ScreenShotDir() / TEXT("VisualizeTexture"));

		uint32 ExtendXWithMSAA = Bitmap.Num() / TextureSizeY;

		// Save the contents of the array to a bitmap file. (24bit only so alpha channel is dropped)
		FFileHelper::CreateBitmap(*ScreenFileName, ExtendXWithMSAA, TextureSizeY, Bitmap.GetData());

//		UE_LOG(LogConsoleResponse, Display, TEXT("Content was saved to "%s""), *FPaths::ScreenShotDir());
	}
	else
	{
//		UE_LOG(LogConsoleResponse, Error, TEXT("Failed to save BMP, format or texture type is not supported"));
	}
}

void Set_Material_via_RenderTarget_Actor(AActor* HitActor, UTextureRenderTarget2D* RenderTarget, UMaterialInterface* MaterialToApplyToClickedObject)
{
	//Try to set a texture to the object we hit!
	if (NULL != HitActor) {
		TArray<UStaticMeshComponent*> StaticMeshComponents = TArray<UStaticMeshComponent*>();
		HitActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

		for (int32 i = 0; i < StaticMeshComponents.Num(); i++) {
			UStaticMeshComponent* CurrentStaticMeshPtr = StaticMeshComponents[i];
			CurrentStaticMeshPtr->SetMaterial(0, MaterialToApplyToClickedObject);
			UMaterialInstanceDynamic* MID =
				CurrentStaticMeshPtr->CreateAndSetMaterialInstanceDynamic(0);
			UTexture* CastedRenderTarget = CastRenderTargetToTexture(RenderTarget);
			MID->SetTextureParameterValue("InputTexture", CastedRenderTarget);
		}
	}
}
