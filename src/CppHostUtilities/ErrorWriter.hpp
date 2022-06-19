#pragma once

class ErrorWriter final
{
public:
	static void Register( fxrSetErrorWriter_f setErrorWriter );
	static void HOSTFXR_CALLTYPE ErrorWriteFunction( const char_t* message );
};
