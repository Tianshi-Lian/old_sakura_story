#include <core/types.h>

#include <debug/logger.h>
#include <debug/instrumentor.h>

#include <network/message.h>
#include <network/client/client.h>
#include <network/server/server.h>

#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <SDL2/SDL.h>
#include <SDL2_gpu/SDL_gpu.h>

using namespace sita;

void gameMain() {
	const u32 screenWidth = 1600;
	const u32 screenHeight = 900;

	//Log::initialize();
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

			//ImGui_ImplSDL2_ProcessEvent(&event);
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
		game.processMessages();

		renderer.clear();
		game.render(&renderer);
		renderer.display();*/
	}

	Log::debug("Sakura Story shutting down...");
	PROFILE_END_SESSION();
}

enum Message_Types : u32 {
	Server_GetStatus,
	Server_GetPing,
	Server_Register,
	Server_Unregister,

	Client_Accepted,
	Client_AssignID,

	MessageAllClients,
};

class Game_Server : public network::server::Server {
public:
	Game_Server(u16 port) : Server(port) {}

	bool onClientConnect(std::shared_ptr<network::Connection> client) override {
		network::Message message;
		message.header.id = Message_Types::Client_Accepted;
		client->send(message);

		return true;
	}

	void onClientDisconnect(std::shared_ptr<network::Connection> client) override {
		Log::info("[Server] Client disconnected, ID: {}", client->getId());
	}

	void onMessage(std::shared_ptr<network::Connection> client, network::Message& message) override {
		switch (message.header.id) {
			case Message_Types::Server_GetPing: {
				Log::info("[Server] Received ping from client: {}", client->getId());
				client->send(message);
			}
											  break;

			case Message_Types::Server_Register: {
				Log::info("[Server] Registering client: {}", client->getId());
				network::Message assignMessage;
				assignMessage.header.id = Message_Types::Client_AssignID;
				assignMessage << client->getId();
				client->send(assignMessage);
			}
											   break;

			case Message_Types::MessageAllClients: {
				Log::info("[Server] Message all from client: {}", client->getId());
				message << client->getId();
				messageAllClients(message, client);
			}
		}
	}
};

class Game_Client : public network::client::Client {
public:
	void pingServer() {
		network::Message message;
		message.header.id = Message_Types::Server_GetPing;

		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

		message << now;
		send(message);
	}

	void messageAll() {
		network::Message message;
		message.header.id = Message_Types::MessageAllClients;
		send(message);
	}
};

int main(int argc, char* argv[]) {
	Log::initialize();

	std::string type = "--client";
	if (argc >= 2) {
		char* ctype = argv[1];
		type = ctype;
	}

	std::thread serverThread = std::thread(
		[]() {
			Game_Server server(60000);
			server.start();

			while (1) {
				server.processMessages();
			}
		}
	);

	std::thread clientThread = std::thread(
		[]() {
			Game_Client client1;
			client1.connect("127.0.0.1", 60000);

			bool key[3] = { false, false, false };
			bool oldKey[3] = { false, false, false };

			bool running = true;
			while (running) {
				if (GetForegroundWindow() == GetConsoleWindow()) {
					key[0] = GetAsyncKeyState('1') & 0x8000;
					key[1] = GetAsyncKeyState('2') & 0x8000;
					key[2] = GetAsyncKeyState('3') & 0x8000;
				}

				if (key[0] && !oldKey[0]) {
					client1.pingServer();
				}
				if (key[1] && !oldKey[1]) {
					client1.messageAll();
				}
				if (key[2] && !oldKey[2]) {
					running = false;
				}

				for (int i = 0; i < 3; ++i) {
					oldKey[i] = key[i];
				}

				if (client1.isConnected()) {
					if (!client1.incoming().isEmpty()) {
						auto message = client1.incoming().popFront().message;

						switch (message.header.id) {
							case Message_Types::Server_GetPing: {
								std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
								std::chrono::system_clock::time_point then;
								message >> then;

								Log::info("[{}] Ping: {}", client1.getId(), std::chrono::duration<double>(now - then).count());
							}
															  break;

							case Message_Types::MessageAllClients: {
								u32 clientId;
								message >> clientId;

								Log::info("[{}] Received message from client: {}", client1.getId(), clientId);
							}
																 break;

							case Message_Types::Client_Accepted: {
								Log::info("[Unknown] Server accepted request");
								network::Message msgRegister;
								msgRegister.header.id = Message_Types::Server_Register;
								client1.send(msgRegister);
							}
															   break;

							case Message_Types::Client_AssignID: {
								u32 clientId;
								message >> clientId;
								client1.setId(clientId);
								Log::info("[{}] Client registered with ID: {} ", client1.getId(), client1.getId());
							}
															   break;
						}
					}
				}
				else {
					Log::info("[{}] Server down!", client1.getId());
					running = false;
				}
			}
		}
	);

	if (serverThread.joinable()) {
		serverThread.join();
	}
	if (clientThread.joinable()) {
		clientThread.join();
	}


	return (0);
}
