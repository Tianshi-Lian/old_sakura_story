#include "common/common.h"

#include "common/debug/logger.h"
#include "common/debug/instrumentor.h"

#define ASIO_STANDALONE
#include <asio/asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <SDL2/SDL.h>
#include <SDL2_gpu/SDL_gpu.h>

#include <chrono>
#include <iostream>

#include "common/net/message.h"

void gameMain() {
	const u32 screenWidth = 1600;
	const u32 screenHeight = 900;

	Log::initialize();
	Log::debug("Project_S Initializing...");

	PROFILE_BEGIN_SESSION("project_s", "logs/timings.json");

	GPU_Target* screen = GPU_Init(screenWidth, screenHeight, GPU_DEFAULT_INIT_FLAGS);
	SDL_Window* window = SDL_GetWindowFromID(screen->context->windowID);
	SDL_SetWindowTitle(window, "Sakura Story");

	//Renderer2D renderer;
	//renderer.initialize(screenWidth, screenHeight, screen);

	//Game game;
	//game.initialize();

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	// Init imgui test
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 410");

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

			ImGui_ImplSDL2_ProcessEvent(&event);
		}
		GPU_Clear(screen);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();
		SDL_GL_MakeCurrent(window, glContext);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		GPU_Flip(screen);
		
		/*
		game.handleInput();
		game.update();

		renderer.clear();
		game.render(&renderer);
		renderer.display();*/
	}

	Log::debug("Sakura Story shutting down...");
	PROFILE_END_SESSION();
}

std::vector<char> g_buffer(1024*1024);

void readData(asio::ip::tcp::socket& socket) {
	socket.async_read_some(asio::buffer(g_buffer.data(), g_buffer.size()),
		[&socket] (std::error_code errorCode, std::size_t length) {
			if (!errorCode) {
				Log::info("readData - Read {} bytes", length);
				for (int i = 0; i < length; ++i) {
					std::cout << g_buffer[i];
				}

				readData(socket);
			}
			else {
				Log::error("readData - Error reading from socket: {}", errorCode.message());
			}
		}	
	);
}

enum Message_Types : u32 {
	FireBullet = 1,
	MovePlayer
};

int main(int argc, char* argv[]) {
	Log::initialize();
	
	asio::error_code errorCode;

	// Context - platform specific interface for asio
	asio::io_context context;
	// Supply idle work so context never finishes
	asio::io_context::work idleWork(context);
	// Start the context running in it's own thread
	std::thread netThread = std::thread([&context]() { context.run(); });

	// Create an endpoint to connect to
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", errorCode), 80);
	// Create a socket to listen to
	asio::ip::tcp::socket socket(context);

	// Connect our socket to the endpoint
	socket.connect(endpoint, errorCode);

	if (!errorCode) {
		Log::info("Connected");
	}
	else {
		Log::error("Failed to connect: {}", errorCode.message());
	}

	if (socket.is_open()) {
		readData(socket);
		
		std::string request = 
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(request), errorCode);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5s);

		context.stop();
		if (netThread.joinable()) {
			netThread.join();
		}
	}

	struct pos {
		float x;
		float y;
	};
	
	sakura::common::net::Message msg;
	msg.header.id = Message_Types::FireBullet;

	msg << 1 << true << 3.0f << pos{2.6f, -4.8f};

	Log::info("Message: {}", msg);

	int a;
	bool b;
	f32 c;
	pos d;

	msg >> d >> c >> b >> a;

	Log::info("a: {}, b: {}, c: {}, d: {},{}", a, b, c, d.x, d.y);

	return (0);
}
