
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

class FComputeShaderPluginImpl : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FComputeShaderPluginImpl & Get()
	{
		return FModuleManager::LoadModuleChecked< FComputeShaderPluginImpl >("ComputeShaderPlugin");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ComputeShaderPlugin");
	}
};
//Declare our plugin module we want to reference
IMPLEMENT_MODULE(FComputeShaderPluginImpl, ComputeShaderPlugin)



void FComputeShaderPluginImpl::StartupModule()
{

	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ComputeShaderDevPlugin"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/ComputeShaderDev"), PluginShaderDir);
}

void FComputeShaderPluginImpl::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}






