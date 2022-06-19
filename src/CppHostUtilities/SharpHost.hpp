#pragma once

// Useful stuff:
// https://docs.microsoft.com/en-us/dotnet/core/tutorials/netcore-hosting
// https://github.com/dotnet/runtime/blob/main/docs/design/features/native-hosting.md

// nethost.h, coreclr_delegates.h and hostfxr.h are at first confusing to find
// You can find them in the example code from the .NET Core hosting tutorial, or:
//
// Simply download some packages via commandline NuGet:
// nuget.exe install runtime.win-x64.Microsoft.NETCore.DotNetAppHost
// There are also win-x86 and linux-x64. Doesn't look like there's an x86 Linux one, so oof
// It might be possible to build an x86 one though: https://github.com/dotnet/runtime/tree/main/src/native/corehost
#include "nethost.h"
#include "coreclr_delegates.h"
#include "hostfxr.h"

#include <string>
#include <vector>

// Shorten these names
// Used by SharpHost to load function pointers for assemblies
using fxrInit_f = hostfxr_initialize_for_runtime_config_fn;
using fxrGetDelegate_f = hostfxr_get_runtime_delegate_fn;
using fxrClose_f = hostfxr_close_fn;
using fxrSetErrorWriter_f = hostfxr_set_error_writer_fn;

// Used by Assembly to get function pointers to C# methods
using loadAssemblyAndGetFuncPtr_f = load_assembly_and_get_function_pointer_fn;
using getFunctionPointer_f = get_function_pointer_fn;
// Used by Assembly to call methods
using entryPoint_f = component_entry_point_fn;

class Assembly;

class SharpHost final
{
public:
	// TODO: add runtime config parameter to this method
	// That way it'll initialise a hostfxr context, and enable the mixed-mode debugger
	bool				Init();
	void				Shutdown();

	// Loads the managed assembly
	// @param assemblyName: Must not contain the extension!
	// @return nullptr if it cannot be found, otherwise a proper assembly reference
	Assembly*			LoadAssembly( const char* assemblyName, const char* entryPointNamespace = "Library" );

private:
	// Hostfxr loading utility
	bool				LoadHostFxr();

	// HostFxr API
	// Initialises a HostFxr context with a given runtime configuration
	fxrInit_f			fxrInit{ nullptr };
	// Obtains functions from NetHost, kinda like GetProcAddress or dlopen
	fxrGetDelegate_f	fxrGetDelegate{ nullptr };
	// Closes an fxr context
	fxrClose_f			fxrClose{ nullptr };
	// Sets the logger for whatever error messages we get from .NET
	fxrSetErrorWriter_f fxrSetErrorWriter{ nullptr };

	// Path of this executable
	std::string			currentDirectory;

	std::vector<Assembly> assemblies;
};
