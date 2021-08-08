#include "client/net/client.h"

#include "common/debug/logger.h"
#include "common/net/connection.h"

using namespace sakura;
using namespace common::core;
using namespace client::net;

Client::~Client() {
	disconnect();
}

bool Client::connect(const std::string& host, const uint16_t port) {
	try {
		asio::ip::tcp::resolver resolver(m_context);
		asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

		m_connection = std::make_unique<common::net::Connection>(common::net::Connection::Owner::CLIENT, m_context, asio::ip::tcp::socket(m_context), m_messagesIn);

		m_connection->connectToServer(endpoints);

		m_contextThread = std::thread([this]() { m_context.run(); });
	}
	catch (std::exception& e) {
		Log::critical("Client::connect - Failed to connect to: {}. Error: {}", host, e.what());
		return false;
	}
	return true;
}

void Client::disconnect() {
	if (isConnected()) {
		m_connection->disconnect();
	}

	m_context.stop();
	if (m_contextThread.joinable())
		m_contextThread.join();

	m_connection.reset();
}

void Client::send(const common::net::Message& msg) const {
	if (isConnected()) {
		m_connection->send(msg);
	}
}

void Client::setId(u32 id) const {
	m_connection->setId(id);
}

bool Client::isConnected() const {
	if (m_connection) {
		return m_connection->isConnected();
	}

	return false;
}

u32 Client::getId() const {
	if (m_connection) {
		return m_connection->getId();
	}
	return 0;
}

Threadsafe_Queue<common::net::Owned_Message>& Client::incoming() {
	return m_messagesIn;
}
