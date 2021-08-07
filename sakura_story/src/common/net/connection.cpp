#include "common/net/connection.h"

using namespace sakura::common::net;

Connection::Connection(asio::io_context& context) :
	m_context(context), m_socket(m_context) {
}

bool Connection::connectToServer(asio::ip::tcp::resolver::results_type& endpoints) {
	return false;
}

bool Connection::disconnect() {
	return false;
}

bool Connection::isConnected() const {
	return false;
}

bool Connection::send(const Message& message) {
	return false;
}
