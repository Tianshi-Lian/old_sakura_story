#include "server/net/server.h"

#include "common/debug/logger.h"

using namespace sakura::server::net;


Server::Server(uint16_t port) :
	m_acceptor(m_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
}

Server::~Server() {
	stop();
}

bool Server::start() {
	try {
		waitForClientConnection();

		m_contextThread = std::thread([this]() { m_context.run(); });
	}
	catch (std::exception& e) {
		Log::critical("[Server] Server::start - Failed to start: {}", e.what());
		return false;
	}

	Log::info("[Server] Server started.");
	return true;
}

void Server::stop() {
	m_context.stop();

	if (m_contextThread.joinable()) m_contextThread.join();

	Log::info("[Server] Server stopped.");
}

void Server::waitForClientConnection() {
	m_acceptor.async_accept(
		[this](std::error_code errorCode, asio::ip::tcp::socket socket) {
			if (!errorCode) {
				Log::info("[Server] New connection: {}", socket.remote_endpoint());

				std::shared_ptr<common::net::Connection> newConnection = std::make_shared<common::net::Connection>(
					common::net::Connection::Owner::SERVER, m_context, std::move(socket), m_MessagesIn);

				if (onClientConnect(newConnection)) {
					m_connections.push_back(std::move(newConnection));

					m_connections.back()->connectToClient(nIDCounter++);

					Log::info("[Server] Connection Approved for client: {}", m_connections.back()->getId());
				}
				else {
					Log::warn("[Server] Connection denied: {}", socket.remote_endpoint());
				}
			}
			else {
				Log::error("[Server] Failed to connect: {}. Error: {}", socket.remote_endpoint(), errorCode.message());
			}

			waitForClientConnection();
		}
	);
}

void Server::messageClient(std::shared_ptr<common::net::Connection> client, const common::net::Message& message) {
	if (client && client->isConnected()) {
		client->send(message);
	}
	else {
		onClientDisconnect(client);

		client.reset();

		m_connections.erase(std::remove(m_connections.begin(), m_connections.end(), client), m_connections.end());
	}
}

void Server::messageAllClients(const common::net::Message& message, const std::shared_ptr<common::net::Connection>& ignoredClient) {
	bool invalidClient = false;

	for (auto& client : m_connections) {
		if (client && client->isConnected()) {
			if (client != ignoredClient)
				client->send(message);
		}
		else {
			onClientDisconnect(client);
			client.reset();

			invalidClient = true;
		}
	}

	if (invalidClient) {
		m_connections.erase(std::remove(m_connections.begin(), m_connections.end(), nullptr), m_connections.end());
	}
}

void Server::processMessages(u64 maxMessages, bool wait) {
	if (wait) m_MessagesIn.wait();

	size_t messageCount = 0;
	while (messageCount < maxMessages && !m_MessagesIn.isEmpty()) {
		auto message = m_MessagesIn.popFront();

		onMessage(message.remote, message.message);

		messageCount++;
	}
}

bool Server::onClientConnect(std::shared_ptr<common::net::Connection> client) {
	return false;
}

void Server::onClientDisconnect(std::shared_ptr<common::net::Connection> client) {
}

void Server::onMessage(std::shared_ptr<common::net::Connection> client, common::net::Message& message) {
}

