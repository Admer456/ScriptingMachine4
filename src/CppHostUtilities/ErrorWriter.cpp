
#include "SharpHost.hpp"
#include "ErrorWriter.hpp"
#include <iostream>

void ErrorWriter::Register( fxrSetErrorWriter_f setErrorWriter )
{
	if ( setErrorWriter )
	{
		setErrorWriter( &ErrorWriter::ErrorWriteFunction );
		std::cout << "ErrorWriter::Register: success" << std::endl;
	}
	else
	{
		std::cout << "ErrorWriter::Register: setErrorWriter is nullptr" << std::endl;
	}
}

void HOSTFXR_CALLTYPE ErrorWriter::ErrorWriteFunction( const char_t* message )
{
	std::wcout << L"hostfxr: < " << message << L" >" << std::endl;
}
