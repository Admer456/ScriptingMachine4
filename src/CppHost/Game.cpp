
#include "Game.hpp"
#include "GameCallbacks.hpp"

#include "../CppHostUtilities/SharpHost.hpp"
#include "../CppHostUtilities/Assembly.hpp"

#include <iostream>

#include "Timer.hpp"

SharpHost sharpHost;
SDL_Renderer* gameRenderer; // extern'ed in GameCallbacks.cpp
Assembly* gameLibrary;
GameImports managedGame;
bool GameInit( SDL_Renderer* r )
{
	// Initialise hostfxr - any breakpoint before this is done won't be hit
	sharpHost.Init();
	// Load C# game DLL
	gameLibrary = sharpHost.LoadAssembly( "net5.0/Library", "Library" );

	gameRenderer = r;

	// Call the "entry point" just to say hello, this is entirely optional
	if ( gameLibrary )
	{
		gameLibrary->CallEntryPoint();
	}
	else
	{
		std::cout << "Couldn't load Library.dll" << std::endl;
		return false;
	}

	// Pass our functions to the assembly
	GameExports ge; // all default values already point to GameCallbacks::xyz
	gameLibrary->CallStructMethod( "Library", "EngineInterface", "ImportFunctions", &ge );
	// ManagedGame contains all the methods that we'll be directly calling over here from the C++ side
	managedGame = ge.ManagedGame;

	// Spawn a simple little entity
	SpawnEntity<Cursor>( 800, 300 );

	// Call GameInit, maybe the C# DLL wants to spawn new entities or something
	gameLibrary->CallVoidMethod( "Library", "EngineInterface", "GameInit" );

	return true;
}

void GameEventAdd( EntityEvent ev )
{
	gameEvents.push_back( ev );

	managedGame.ReceiveEvent( ev );
}

void GameLoop( SDL_Renderer* r )
{
	for ( GameEntity*& e : entities )
	{
		if ( nullptr == e )
			continue;

		if ( e->flags & GameEntity::Flag_Remove )
		{
			delete e;
			e = nullptr;
			continue;
		}

		for ( EntityEvent& eventId : gameEvents )
		{
			if ( ~e->flags & GameEntity::Flag_NoEvents )
			{
				e->ReceiveEvent( eventId );
			}
		}

		if ( ~e->flags & GameEntity::Flag_NoThink )
		{
			e->Think();
		}

		if ( ~e->flags & GameEntity::Flag_NoPresent )
		{
			e->Present( r );
		}
	}

	Timer callingTime( true );
	if ( gameLibrary )
	{
		gameLibrary->CallVoidMethod( "Library", "EngineInterface", "GameLoop" );
	}
	float ms = callingTime.GetElapsed( Timer::Milliseconds );
	//std::cout << "Took " << ms << "ms to call Library.EngineInterface.GameLoop" << std::endl;

	gameEvents.clear();
}

void GameUpdateState( float mx, float my, bool click )
{
	mouseX = mx;
	mouseY = my;
	mouseClickOld = mouseClick;
	mouseClick = click;

	if ( mouseClickOld != mouseClick )
	{
		mouseClickLatch = true;
	}
	else
	{
		mouseClickLatch = false;
	}
}

void GameShutdown()
{
	gameLibrary->CallVoidMethod( "Library", "EngineInterface", "GameShutdown" );
	sharpHost.Shutdown();
}
