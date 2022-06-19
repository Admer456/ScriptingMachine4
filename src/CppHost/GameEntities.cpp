
#include "Game.hpp"

// =====================================
// Game entity implementation
// =====================================
void GameEntity::Present( SDL_Renderer* r )
{
	SDL_Rect rect;
	rect.x = x - radius * 0.5f;
	rect.y = y - radius * 0.5f;
	rect.h = radius;
	rect.w = radius;

	SDL_SetRenderDrawColor( r, red * 0.8f, green * 0.8f, blue * 0.8f, 255 );
	SDL_RenderDrawRect( r, &rect );

	float vx = cos( yaw ) * radius * 0.5f;
	float vy = sin( yaw ) * radius * 0.5f;

	rect.x = x + vx - innerRadius * 0.5f;
	rect.y = y + vy - innerRadius * 0.5f;
	rect.h = innerRadius;
	rect.w = innerRadius;

	SDL_SetRenderDrawColor( r, red, green, blue, 255 );
	SDL_RenderDrawRect( r, &rect );
}

void GameEntity::SendEvent( int eventId, int x, int y )
{
	if ( ~flags & Flag_NoEvents )
	{
		GameEventAdd( { eventId, x, y } );
	}
}

float GameEntity::LengthFrom( int tx, int ty )
{
	float dx = tx - x;
	float dy = ty - y;

	return sqrt( (dx * dx) + (dy * dy) );
}

// =====================================
// Simple rotating entity
// =====================================
class Rotating : public GameEntity
{
public:
	void Think() override
	{
		yaw += 0.016f;
	}
};

// =====================================
// Virtual cursor that follows the mouse
// =====================================
void Cursor::Think()
{
	if ( mouseClick )
	{
		SendEvent( Event_MouseClick, x, y );
	}
}

void Cursor::Present( SDL_Renderer* r )
{
	if ( mouseClick )
	{
		ox = ox * 0.8f + x * 0.2f;
		oy = oy * 0.8f + y * 0.2f;
	}
	else
	{
		ox = ox * 0.95f + x * 0.05f;
		oy = oy * 0.95f + y * 0.05f;
	}

	x = mouseX;
	y = mouseY;

	SDL_SetRenderDrawColor( r, 255, mouseClick ? 0 : 160, 0, 255 );
	SDL_RenderDrawLine( r, ox, oy, x, y );
}

// =====================================
// Particle entity that flies in a direction then fades away
// TODO: expose to the C# side somehow
// =====================================
class Particle : public GameEntity
{
public:
	static Particle* Create( float x, float y, float yaw, float speed )
	{
		Particle* p = SpawnEntity<Particle>( x, y );
		p->ox = x;
		p->oy = y;
		p->yaw = yaw;
		p->speed = speed;

		return p;
	}

	void Think() override
	{
		ox = ox * 0.92f + x * 0.08f;
		oy = oy * 0.92f + y * 0.08f;

		float vx = cos( yaw ) * speed * 0.016f;
		float vy = sin( yaw ) * speed * 0.016f;

		x += vx;
		y += vy;

		speed /= 1.10f;

		if ( speed < 1.0f )
		{
			flags |= Flag_Remove;
		}
	}

	void Present( SDL_Renderer* r ) override
	{
		red = 255.0f;
		green = 160.0f;
		blue = 64.0f;

		if ( speed < 100.0f )
		{
			red *= (speed / 100.0f);
			green *= (speed / 250.0f);
			blue *= (speed / 550.0f);
		}

		SDL_SetRenderDrawColor( r, red, green, blue, 255 );
		SDL_RenderDrawLine( r, ox, oy, x, y );
	}

	float speed{ 0.0f };
	float ox{ 0.0f }, oy{ 0.0f };
};

// =====================================
// Explosive box, reacts to being clicked by the mouse
// =====================================
class Explosive : public GameEntity
{
public:
	Explosive() : GameEntity()
	{
		for ( Particle*& p : particles )
		{
			p = Particle::Create( x, y, Random() * 360.0f, 50.0f + Random() * 2000.0f );
			p->flags = Flag_NoThink | Flag_NoPresent;
		}
	}

	void Think() override
	{
		if ( !alive )
			return;

		if ( mouseClick )
		{
			if ( mouseX < (x + (radius * 0.5f))
				&& mouseX >( x - (radius * 0.5f) )
				&& mouseY < (y + (radius * 0.5f))
				&& mouseY >( y - (radius * 0.5f) ) )
			{
				Explode();
			}
		}
	}

	void Present( SDL_Renderer* r ) override
	{
		if ( !alive )
			return;

		float halfRadius = radius * 0.5f;

		red = 255;
		green = 32;
		blue = 0;

		SDL_SetRenderDrawColor( r, red, green, blue, 255 );
		SDL_RenderDrawLine( r, x - halfRadius, y - halfRadius, x + halfRadius, y + halfRadius );
		SDL_RenderDrawLine( r, x - halfRadius, y + halfRadius, x + halfRadius, y - halfRadius );

		GameEntity::Present( r );
	}

	void ReceiveEvent( EntityEvent& ev ) override
	{
		switch ( ev.eventId )
		{
		case Event_Explosion:
		{
			if ( LengthFrom( ev.x, ev.y ) < (radius * 2.0f) )
			{
				Explode();
				return;
			}
		} break;
		}
	}

	void Explode()
	{
		// Release the particles
		for ( Particle*& p : particles )
		{
			p->flags = 0;
			p->x = x;
			p->y = y;
			p->ox = x;
			p->oy = y;
		}

		SendEvent( Event_Explosion, x, y );

		flags |= Flag_Remove | Flag_NoEvents;
	}

	bool alive = true;
	Particle* particles[50];
};
