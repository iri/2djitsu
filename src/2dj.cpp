#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <chrono>
#include <thread>
#include <data.hpp>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

bool init();

bool loadMedia();

void close();

SDL_Texture* loadTexture( std::string path );

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

bool init()
{
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		gWindow = SDL_CreateWindow( "Simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;

	return success;
}

void close()
{
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture( std::string path )
{
	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}


int main( int argc, char* args[] )
{	
	auto M = std::make_unique<Model>();

	M->readConfig("model.yaml");

	M->addVehicle(std::make_unique<Vehicle>(2, 4, 0.5,0.5, 0,255,0,200, *M));
	M->addVehicle(std::make_unique<Vehicle>(2, 4, 0.5,0.5, 255,0,0,200, *M));
	M->addVehicle(std::make_unique<Vehicle>(2, 4, 0.5,0.5, 0,0,255,200, *M));
	M->addVehicle(std::make_unique<Vehicle>(2, 4, 0.5,0.5, 255,255,0,255, *M));

	M->setVehicle( 0, 0, 0, 45, 25, 0.05, 0 );
	M->setVehicle( 1, 0, 6, 120, 20, 0.1, 0 );
	M->setVehicle( 2, -3, -7, 90, 15, 0.07, 0 );
	M->setVehicle( 3, 4, -7, 140, -15, 0.02, 0 );

	M->addView(std::make_unique<View>(0,0,30,40,SCREEN_WIDTH,SCREEN_HEIGHT,*M));

	M->display();

	M->start();

//=================================================================================

	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			std::chrono::steady_clock::time_point t0,t;
			t0 = std::chrono::steady_clock::now();
			long long elapsed;
			
			bool quit = false;

			SDL_Event e;

			while( !quit )
			{
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
				}

				// Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xCC, 0xCC, 0xCC, 0xFF );
				SDL_RenderClear( gRenderer );

				// Render model view scene
				M->render(gRenderer);

				// Update screen
				SDL_RenderPresent( gRenderer );

				do {
					std::this_thread::sleep_for(std::chrono::microseconds(1000));
					t = std::chrono::steady_clock::now();
					elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t-t0).count();
				} while ( elapsed < 10000 );  // 100FPS

		        // std::cout << "FPS: " << 1 / 0.000001 / elapsed << std::endl;
				t0 = t;

			}
		}
	}

	// Stop model
	M->exit();

	// Free resources
	close();

	return 0;
}

