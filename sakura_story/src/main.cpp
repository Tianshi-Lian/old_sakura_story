#include "common/types.h"

#include "common/debug/logger.h"
#include "common/debug/instrumentor.h"

#include <SDL2/SDL.h>
#include <SDL2_gpu//SDL_gpu.h>

int main(int argc, char* argv[]) {
	const u32 screenWidth = 1600;
	const u32 screenHeight = 900;

	Log::initialize();
	Log::debug("Project_S Initializing...");

	PROFILE_BEGIN_SESSION("project_s", "logs/timings.json");

	GPU_Target* screen = GPU_Init(screenWidth, screenHeight, GPU_DEFAULT_INIT_FLAGS);
	SDL_SetWindowTitle(SDL_GetWindowFromID(screen->context->windowID), "Project S");

	//Renderer2D renderer;
	//renderer.initialize(screenWidth, screenHeight, screen);

	//Game game;
	//game.initialize();

	bool running = true;
	SDL_Event event;
	while (running) {
		PROFILE_SCOPE("gameLoop");

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				/*if (game.getLang() == Language::EN) {
					game.setLang(Language::DE);
				}
				else {
					game.setLang(Language::EN);
				}*/
			}
		}
		/*
		game.handleInput();
		game.update();

		renderer.clear();
		game.render(&renderer);
		renderer.display();*/
	}

	Log::debug("Sakura Story shutting down...");
	PROFILE_END_SESSION();

	return (0);
}
