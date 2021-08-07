#include "client/net/client.h"

#include "common/net/connection.h"

using namespace sakura;
using namespace common::core;
using namespace client::net;

Client::Client() :
	m_socket(m_context), m_connection() {
}

Client::~Client() {
	disconnect();
}

bool Client::connect(const std::string& host, u16 port) {
	bool result = true;
	
	try {
		m_connection = std::make_unique<common::net::Connection>(m_context);

		asio::ip::tcp::resolver resolver(m_context);
		asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

		m_connection->connectToServer(endpoints);

		m_contextThread = std::thread([this]() {
			m_context.run();
		});
	}
	catch (const std::exception& e) {
		Log::error("Client::connect - connection error: {}", e.what());
		result = false;
	}

	return result;
}

bool Client::disconnect() {
	bool result = false;
	
	if (isConnected()) {
		result = m_connection->disconnect();
	}

	m_context.stop();
	if (m_contextThread.joinable()) {
		m_contextThread.join();
	}

	m_connection.reset();

	return result;
}

bool Client::isConnected() const {
	if (m_connection) {
		return m_connection->isConnected();
	}

	return false;
}

Threadsafe_Queue<common::net::Owned_Message>& Client::incoming() {
	return m_messagesIn;
}
