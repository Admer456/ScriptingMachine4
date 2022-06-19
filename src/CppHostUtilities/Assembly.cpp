
#include "SharpHost.hpp"
#include "Assembly.hpp"
#include "Utility.hpp"

Assembly::Assembly( const char* assemblyName, const char* assemblyPath, ConstructionParams fxrParams )
	: fullPath( assemblyPath ), libraryName( assemblyName ), libraryNamespace( fxrParams.namespaceName ),
	fxrLoadAssemblyAndGetFunctionPointer( fxrParams.fxrLoadAssemblyAndGetFuncPtr )
{
	// the runtime config needs to be a wstring
	// hostfxr functions require that
	std::string runtimeConfigPath = fullPath + ".runtimeconfig.json";
	StringToWString( runtimeConfigPath, runtimeConfig );

	// Go on to actually initialise this
	Init( fxrParams );
}

const char* Assembly::GetName() const
{
	return libraryName.c_str();
}

// Todo: Method class which lets us store and call methods without
// having to load function pointers every time
// Also have them be templates so we can put custom methods/delegate types
void Assembly::CallStructMethod( const char* nameSpace, const char* className, const char* methodName, void* args )
{
	using structMethod_f = void CORECLR_DELEGATE_CALLTYPE ( void* args );
	structMethod_f* structMethod = nullptr;

	std::string typeName = std::string( nameSpace ) + "." + className + ", " + nameSpace;
	std::string delegateTypeName = std::string( nameSpace ) + "." + className + "+" + methodName + "Delegate, " + nameSpace;

	std::wstring typeNameW;
	std::wstring methodNameW;
	std::wstring delegateTypeNameW;

	StringToWString( typeName, typeNameW );
	StringToWString( methodName, methodNameW );
	StringToWString( delegateTypeName, delegateTypeNameW );

	int resultCode = fxrLoadAssemblyAndGetFunctionPointer(
		fullPathW.c_str(),
		typeNameW.c_str(), // type_name
		methodNameW.c_str(), // method_name
		delegateTypeNameW.c_str(), // delegate_type_name
		nullptr, // reserved
		(void**)&structMethod // delegate
	);

	if ( resultCode || nullptr == structMethod )
	{
		return;
	}

	structMethod( args );
}

void Assembly::CallVoidMethod( const char* nameSpace, const char* className, const char* methodName )
{
	using voidMethod_f = void CORECLR_DELEGATE_CALLTYPE ();
	voidMethod_f* voidMethod = nullptr;

	std::string typeName = std::string( nameSpace ) + "." + className + ", " + nameSpace;
	std::string delegateTypeName = std::string( nameSpace ) + "." + className + "+" + methodName + "Delegate, " + nameSpace;

	std::wstring typeNameW;
	std::wstring methodNameW;
	std::wstring delegateTypeNameW;

	StringToWString( typeName, typeNameW );
	StringToWString( methodName, methodNameW );
	StringToWString( delegateTypeName, delegateTypeNameW );

	int resultCode = fxrLoadAssemblyAndGetFunctionPointer(
		fullPathW.c_str(),
		typeNameW.c_str(), // type_name
		methodNameW.c_str(), // method_name
		delegateTypeNameW.c_str(), // delegate_type_name
		nullptr, // reserved
		(void**)&voidMethod // delegate
	);

	if ( resultCode || nullptr == voidMethod )
	{
		return;
	}

	voidMethod();
}

void Assembly::CallEntryPoint()
{
	if ( assemblyEntryPoint )
	{
		assemblyEntryPoint( nullptr, 0 );
	}
}

bool Assembly::IsGood() const
{
	return good;
}

void Assembly::Init( ConstructionParams fxrParams )
{
#ifdef WIN32
	fullPath += ".dll";
#elif UNIX
	fullPath += ".so";
#endif

	// wstrings for hostfxr
	std::wstring libraryNamespaceW;
	
	StringToWString( fullPath, fullPathW );
	StringToWString( libraryNamespace, libraryNamespaceW );

	// Library.Main, Library
	std::wstring typeNameW = libraryNamespaceW + L".MainClass, " + libraryNamespaceW;

	// Load the entry point
	int resultCode = fxrLoadAssemblyAndGetFunctionPointer(
		fullPathW.c_str(), // path/to/library.dll
		typeNameW.c_str(), // Library.Main in namespace Library
		L"Entry",
		nullptr, nullptr,
		(void**)&assemblyEntryPoint
	);

	if ( resultCode || nullptr == assemblyEntryPoint )
	{
		good = false;
		return;
	}

	// All succeeded, we're good
	good = true;
}
