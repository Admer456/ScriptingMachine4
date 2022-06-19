#pragma once

#include <string>
#pragma warning( disable : 4244 )

inline void WStringToString( const std::wstring& in, std::string& out )
{
	out.reserve( in.length() );
	out = "";

	for ( auto& character : in )
	{
		out += character;
	}
}

inline void StringToWString( const std::string& in, std::wstring& out )
{
	out.reserve( in.length() );
	out = L"";

	for ( auto& character : in )
	{
		out += character;
	}
}
