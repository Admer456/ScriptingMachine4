
#include <iostream>
#include "CppHost/Game.hpp"

int main( int argc, char* argv[] )
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event e;

	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );

	window = SDL_CreateWindow( "ScriptingMachine4 uwu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0 );
	renderer = SDL_CreateRenderer( window, 0, SDL_RendererFlags::SDL_RENDERER_SOFTWARE );

	if ( GameInit( renderer ) )
	{
		float mouseX{ 0.0f }, mouseY{ 0.0f };
		bool leftClick = false;

		bool quit = false;
		while ( !quit )
		{
			while ( SDL_PollEvent( &e ) )
			{
				if ( e.type == SDL_QUIT )
				{
					quit = true;
				}

				if ( e.type == SDL_MOUSEMOTION )
				{
					mouseX = e.motion.x;
					mouseY = e.motion.y;
				}

				if ( e.type == SDL_MOUSEBUTTONDOWN )
				{
					if ( e.button.button == SDL_BUTTON_LEFT )
					{
						leftClick = true;
					}
				}

				if ( e.type == SDL_MOUSEBUTTONUP )
				{
					if ( e.button.button == SDL_BUTTON_LEFT )
					{
						leftClick = false;
					}
				}
			}

			SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
			SDL_RenderClear( renderer );

			GameUpdateState( mouseX, mouseY, leftClick );
			GameLoop( renderer );

			SDL_RenderPresent( renderer );
			SDL_Delay( 16 );
		}

		GameShutdown();
	}

	SDL_Quit();

	return 0;
}
