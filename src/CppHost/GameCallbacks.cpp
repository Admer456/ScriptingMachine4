
#include "Game.hpp"
#include "GameCallbacks.hpp"

uint32_t GameCallbacks::CreateEntity( int x, int y )
{
	GameEntity* ent = SpawnEntity<GameEntity>( x, y );
	if ( nullptr == ent )
	{
		return 0;
	}

	return ent->index;
}

void GameCallbacks::RemoveEntity( uint32_t entityIndex )
{
	if ( entityIndex < 0 || entityIndex >= (1 << 16U) )
	{
		return;
	}

	GameEntity* entity = entities[entityIndex];
	
	if ( nullptr != entity )
	{
		entity->flags = GameEntity::Flag_Remove;
	}
}

// Game.cpp
extern SDL_Renderer* gameRenderer;

void GameCallbacks::SetRenderColor( char red, char green, char blue )
{
	SDL_SetRenderDrawColor( gameRenderer, red, green, blue, 255 );
}

void GameCallbacks::DrawLine( int x1, int y1, int x2, int y2 )
{
	SDL_RenderDrawLine( gameRenderer, x1, y1, x2, y2 );
}

void GameCallbacks::SendEvent( const EntityEvent& ev )
{
	gameEvents.push_back( ev );
}
