#pragma once

// Actual game callbacks, that will be exported to the managed game
class GameCallbacks final
{
public:
	static uint32_t		CreateEntity( int x, int y );
	static void			RemoveEntity( uint32_t entityIndex );
	static void			SetRenderColor( char red, char green, char blue );
	static void			DrawLine( int x1, int y1, int x2, int y2 );
	static void			SendEvent( const EntityEvent& ev );
};

#if defined(_WIN32)
#define DELEGATE __stdcall
#else
#define DELEGATE
#endif

// Delegate types, **MUST MATCH** the delegates in the managed game
using ICreateEntity		= uint32_t	/*DELEGATE*/ ( int x, int y );
using IRemoveEntity		= void		/*DELEGATE*/ ( uint32_t entityIndex );
using ISetRenderColor	= void		/*DELEGATE*/ ( char red, char green, char blue );
using IDrawLine			= void		/*DELEGATE*/ ( int x1, int y1, int x2, int y2 );
using ISendEvent		= void		/*DELEGATE*/ ( const EntityEvent& ev );

// Stuff we import from the C# game, this'll be GameExports in C#
using IReceiveEvent = void /*DELEGATE*/( const EntityEvent& ev );
struct GameImports
{
	int gameVersion;
	IReceiveEvent* ReceiveEvent{ nullptr };
};

// A structure we'll pass to the managed game's ImportFunctions method
struct GameExports
{
	ICreateEntity*		CreateEntity{	GameCallbacks::CreateEntity };
	IRemoveEntity*		RemoveEntity{	GameCallbacks::RemoveEntity };
	ISetRenderColor*	SetRenderColor{ GameCallbacks::SetRenderColor };
	IDrawLine*			DrawLine{		GameCallbacks::DrawLine };
	ISendEvent*			SendEvent{		GameCallbacks::SendEvent };

	//GameImports*		ManagedGame{ nullptr };
	GameImports			ManagedGame;
};
