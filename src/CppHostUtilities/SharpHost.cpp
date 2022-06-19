
#include "SharpHost.hpp"
#include "Assembly.hpp"
#include "Utility.hpp"
#include "ErrorWriter.hpp"

#include "SDL.h"

bool SharpHost::Init()
{
	// Load hostfxr
	if ( !LoadHostFxr() )
	{
		return false;
	}

	// Set the base path
	currentDirectory = SDL_GetBasePath();

	// Register the error writer
	ErrorWriter::Register( fxrSetErrorWriter );

	return true;
}

void SharpHost::Shutdown()
{
	assemblies.clear();
}

Assembly* SharpHost::LoadAssembly( const char* assemblyName, const char* entryPointNamespace )
{
	// Check if we already got this same assembly
	for ( Assembly& as : assemblies )
	{
		if ( !stricmp( as.GetName(), assemblyName ) )
		{
			return &as;
		}
	}

	// Temporary hostfxr context, required to get function pointers for this assembly
	hostfxr_handle context = nullptr;
	
	// These are function pointers that will be used to construct our assembly handle
	// They're needed so we can get function pointers to static methods from assemblies, so we can call them!
	loadAssemblyAndGetFuncPtr_f loadAssemblyAndGetFuncPtr = nullptr;
	getFunctionPointer_f getFunctionPointer = nullptr;

	// Construct the assembly paths
	std::string assemblyPath = currentDirectory + assemblyName;
	std::string assemblyRuntimeConfig = assemblyPath + ".runtimeconfig.json";
	std::wstring assemblyRuntimeConfigWide;

	// hostfxr functions need wstrings
	StringToWString( assemblyRuntimeConfig, assemblyRuntimeConfigWide );

	// Load .NET Core, set up the context
	int resultCode = fxrInit( assemblyRuntimeConfigWide.c_str(), nullptr, &context );
	if ( resultCode || nullptr == context )
	{
		fxrClose( context );
		return nullptr;
	}

	// Get the "load assembly" function pointer
	resultCode = fxrGetDelegate( context,
								 hdt_load_assembly_and_get_function_pointer,
								 (void**)&loadAssemblyAndGetFuncPtr );
	if ( resultCode || nullptr == loadAssemblyAndGetFuncPtr )
	{
		fxrClose( context );
		return nullptr;
	}

	// Try to get "get function pointer" too
	resultCode = fxrGetDelegate( context,
								 hdt_get_function_pointer,
								 (void**)&getFunctionPointer );
	if ( resultCode || nullptr == getFunctionPointer )
	{
		// nothing
	}

	// Build the assembly handle
	Assembly::ConstructionParams assemblyConstructionParams
	{
		context, loadAssemblyAndGetFuncPtr, entryPointNamespace
	};
	Assembly assembly = Assembly( assemblyName, assemblyPath.c_str(), assemblyConstructionParams );
	
	// If it fails to load from there on
	if ( !assembly.IsGood() )
	{
		fxrClose( context );
		return nullptr;
	}

	// If that succeeded, add this assembly so we can bookkeep it
	assemblies.push_back( assembly );

	// Close the context, we don't need it any more
	fxrClose( context );

	return &assemblies.back();
}

bool SharpHost::LoadHostFxr()
{
	void* hostfxrLibrary;
	std::string hostfxrPath;
	
	// buffer = path to hostfxr
	char_t hostfxrBuffer[_MAX_PATH];
	size_t bufferSize = sizeof( hostfxrBuffer ) / sizeof( char_t );
	
	// Get the path to hosfxr
	int resultCode = get_hostfxr_path( hostfxrBuffer, &bufferSize, nullptr );
	// We failed to get teh path
	if ( resultCode )
	{
		return false;
	}

	// Load the hostfxr library
	WStringToString( hostfxrBuffer, hostfxrPath );
	hostfxrLibrary = SDL_LoadObject( hostfxrPath.c_str() );

	// Get the function pointers
	fxrInit = static_cast<fxrInit_f>( SDL_LoadFunction( hostfxrLibrary, "hostfxr_initialize_for_runtime_config" ) );
	fxrGetDelegate = static_cast<fxrGetDelegate_f>(	SDL_LoadFunction( hostfxrLibrary, "hostfxr_get_runtime_delegate" ) );
	fxrClose =	static_cast<fxrClose_f>( SDL_LoadFunction( hostfxrLibrary, "hostfxr_close" ) );
	fxrSetErrorWriter =	static_cast<fxrSetErrorWriter_f>( SDL_LoadFunction( hostfxrLibrary, "hostfxr_set_error_writer" ) );

	return (fxrInit && fxrGetDelegate && fxrClose);
}
