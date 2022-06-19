#pragma once

class Assembly
{
public:
	struct ConstructionParams
	{
		hostfxr_handle				fxrContext{ nullptr };
		loadAssemblyAndGetFuncPtr_f fxrLoadAssemblyAndGetFuncPtr{ nullptr };
		const char*					namespaceName{ nullptr };
	};
public:
	Assembly( const char* assemblyName, const char* assemblyPath, ConstructionParams fxrParams );

	const char*			GetName() const;
	void				CallStructMethod( const char* nameSpace, const char* className, const char* methodName, void* args );
	void				CallVoidMethod( const char* nameSpace, const char* className, const char* methodName );
	void				CallEntryPoint();

	bool				IsGood() const;

private:
	void				Init( ConstructionParams fxrParams );

	bool				good{ false };
	
	std::string			libraryNamespace; // can be anything
	std::string			libraryName; // library
	std::string			fullPath; // path/to/library
	std::wstring		runtimeConfig; // path/to/library.runtimeconfig.json

	loadAssemblyAndGetFuncPtr_f fxrLoadAssemblyAndGetFunctionPointer{ nullptr };
	
	// stuff that gets used while calling fxrLoadAssemblyAndGetFunctionPointer
	std::wstring		fullPathW;

	// [namespace].MainClass.Entry( IntPtr args, int size );
	entryPoint_f		assemblyEntryPoint{ nullptr };
};
