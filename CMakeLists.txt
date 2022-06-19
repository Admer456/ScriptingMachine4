
cmake_minimum_required( VERSION 3.16 )

project( ScriptingMachine4 )

## C++17 cuz' adm-utils
set( CMAKE_CXX_STANDARD 17 )

## I have a habit of setting a root variable cuz' I'm lazy to type CMAKE_CURRENT_SOURCE_DIR every time
## In projects like these, which aren't meant to be used as dependencies, I prefix stuff with THE_,
## cuz' it's THE stuff, there won't be any other
set( THE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

## SDL2
if( UNIX ) # This code hasn't been tested on Linux, but the framework to make it happen should be there
	find_package( SDL2 REQUIRED )
elseif( WIN32 )
	## Note that I've only included 64-bit SDL2 binaries for Windows
	set( SDL2_INCLUDE_DIRS
		${THE_ROOT}/external/SDL2/include )

	set( SDL2_LIBRARIES
		${THE_ROOT}/external/SDL2/lib/SDL2.lib
		${THE_ROOT}/external/SDL2/lib/SDL2main.lib )
else()
	message( FATAL_ERROR "This platform is not supported" )
endif()

## Nethost and HostFxr
## Nethost is the library that provides .NET hosting functionality, and
## HostFxr simply locates nethost and other relevant libraries
set( NETHOST_INCLUDE_DIRS
		${THE_ROOT}/external/nethost )

if( UNIX )
	message( WARNING "Linux support is uncertain, let's hope libnethost.lib is meant for Linux" )
	set( NETHOST_LIBRARIES
		${THE_ROOT}/external/nethost/lib_linux_x64/libnethost.a )
elseif( WIN32 )
	set( NETHOST_LIBRARIES
		${THE_ROOT}/external/nethost/lib_win_x64/nethost.lib )
endif()

## The sources
## I'm not putting the C# sources here, as C# projects seem to be cross-platform by design
set( THE_SOURCES
	src/CppHost/Game.cpp
	src/CppHost/Game.hpp
	src/CppHost/GameCallbacks.cpp
	src/CppHost/GameCallbacks.hpp
	src/CppHost/GameEntities.cpp
	src/CppHost/Timer.cpp
	src/CppHost/Timer.hpp
	src/CppHostUtilities/Assembly.cpp
	src/CppHostUtilities/Assembly.hpp
	src/CppHostUtilities/ErrorWriter.cpp
	src/CppHostUtilities/ErrorWriter.hpp
	src/CppHostUtilities/SharpHost.cpp
	src/CppHostUtilities/SharpHost.hpp
	src/CppHostUtilities/Utility.hpp
	src/Main.cpp
	)

## Folder organisation
source_group( TREE ${THE_ROOT} FILES ${THE_SOURCES} )

## The .exe
add_executable( ScriptingMachine4 ${THE_SOURCES} )

## Include dirs
target_include_directories( ScriptingMachine4 PRIVATE
	${THE_ROOT}
	${SDL2_INCLUDE_DIRS}
	${NETHOST_INCLUDE_DIRS} )

## Link against SDL2 libs
target_link_libraries( ScriptingMachine4 PRIVATE ${SDL2_LIBRARIES} ${NETHOST_LIBRARIES} )

## Output here
install( TARGETS ScriptingMachine4
	RUNTIME DESTINATION ${THE_ROOT}/bin/
	LIBRARY DESTINATION ${THE_ROOT}/bin/ )

## On Windows, copy SDL2.dll, nethost.dll and the .pdb
if( WIN32 )
	install( FILES 
		${THE_ROOT}/external/SDL2/lib/SDL2.dll
		${THE_ROOT}/external/nethost/lib_win_x64/nethost.dll
		DESTINATION ${THE_ROOT}/bin/ )

	install( FILES $<TARGET_PDB_FILE:ScriptingMachine4> DESTINATION ${THE_ROOT}/bin/ OPTIONAL )
## On Linux, just copy libnethost.so
elseif( UNIX )
	install( FILES
		${THE_ROOT}/external/nethost/lib_linux_x64/libnethost.so
		DESTINATION ${THE_ROOT}/bin )
endif()
