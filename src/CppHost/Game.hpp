#pragma once

#include "SDL.h"
#include <vector>

// Game system stuff

bool GameInit( SDL_Renderer* r );
void GameLoop( SDL_Renderer* r );
void GameUpdateState( float mx, float my, bool click );
void GameShutdown();

// Game entity stuff

// EntityEvent, Events and GameEntity are found
// in both the C++ and C# sources

struct EntityEvent
{
	int eventId;
	int x, y;
};

enum Events
{
	Event_Explosion,
	Event_MouseClick
};

class GameEntity
{
public:
	static constexpr uint8_t Flag_Remove = 1 << 0;
	static constexpr uint8_t Flag_NoThink = 1 << 1;
	static constexpr uint8_t Flag_NoPresent = 1 << 2;
	static constexpr uint8_t Flag_NoEvents = 1 << 3;

	virtual void Think() { }
	virtual void Present( SDL_Renderer* r );

	// Said event gets executed next frame
	virtual void SendEvent( int eventId, int x = 0, int y = 0 );
	virtual void ReceiveEvent( EntityEvent& ev ) { }
	float LengthFrom( int tx, int ty );

	uint8_t red{ 255 }, green{ 255 }, blue{ 255 }, alpha{ 255 };

	uint32_t index{ 0 };
	uint8_t flags{ 0 };

	float yaw{ 0.0f };
	float x{ 0.0f };
	float y{ 0.0f };

	float radius{ 8.0f };
	float innerRadius{ 2.0f };
};

// GameInit spawns a Cursor, so I put it here
class Cursor : public GameEntity
{
	void Think() override;
	void Present( SDL_Renderer* r ) override;

	float ox{ 0.0f }, oy{ 0.0f };
};

constexpr uint32_t MaxEntities = 1 << 12U;
inline GameEntity* entities[MaxEntities];

template< typename entityType >
entityType* SpawnEntity( int x, int y )
{
	int i = 0;
	for ( GameEntity*& e : entities )
	{
		if ( nullptr == e )
		{
			e = new entityType();
			e->x = x;
			e->y = y;
			e->index = i;
			return static_cast<entityType*>(e);
		}
		i++;
	}

	return nullptr;
}

// Game logic stuff

inline float mouseX{ 0.0f }, mouseY{ 0.0f };
inline bool mouseClick = false;
inline bool mouseClickLatch = false;
inline bool mouseClickOld = false;

inline std::vector<EntityEvent> gameEvents;
void GameEventAdd( EntityEvent ev );

// Utilities

// Between 0.0 and 1.0
inline float Random()
{
	return (float)((float)(rand() & 0xffff) / 0xffff);
}

// Between -1.0 and 1.0
inline float CRandom()
{
	return (Random() * 2.0f) - 1.0f;
}

#pragma warning( disable : 4244 )
