#include "common/net/connection.h"

#include "common/debug/logger.h"
#include "common/net/common.h"

using namespace sakura::common::net;

Connection::Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, core::Threadsafe_Queue<Owned_Message>& queueIn) :
	m_context(asioContext), m_socket(std::move(socket)), m_messagesIn(queueIn), m_owner(parent) {
}

void Connection::connectToClient(uint32_t uid) {
	if (m_owner == Owner::SERVER) {
		if (m_socket.is_open()) {
			m_id = uid;
			readHeader();
		}
	}
}

void Connection::connectToServer(const asio::ip::tcp::resolver::results_type& endpoints) {
	if (m_owner == Owner::CLIENT) {
		asio::async_connect(m_socket, endpoints,
			[this](std::error_code errorCode, asio::ip::tcp::endpoint endpoint) {
				if (!errorCode) {
					readHeader();
				}
			}
		);
	}
}

void Connection::disconnect() {
	if (isConnected()) {
		asio::post(m_context, [this]() {
			m_socket.close();
			}
		);
	}
}

void Connection::send(const Message& msg) {
	asio::post(m_context,
		[this, msg]() {
			const bool writingMessage = !m_messagesOut.isEmpty();

			m_messagesOut.pushBack(msg);
			if (!writingMessage) {
				writeHeader();
			}
		}
	);
}

void Connection::startListening() {
}

void Connection::setId(u32 id) {
	if (m_owner == Owner::CLIENT) {
		if (m_id != 0) {
			Log::error("[{}] Connection::setId - attempting to set a second ID for client", m_id);
		}
		else {
			m_id = id;
		}
	}
}

u32 Connection::getId() const {
	return m_id;
}

bool Connection::isConnected() const {
	return m_socket.is_open();
}

void Connection::writeHeader() {
	asio::async_write(m_socket, asio::buffer(&m_messagesOut.front().header, sizeof(Message_Header)),
		[this](std::error_code errorCode, std::size_t length) {
			if (!errorCode) {
				if (!m_messagesOut.front().body.empty()) {
					writeBody();
				}
				else {
					m_messagesOut.popFront();

					if (!m_messagesOut.isEmpty()) {
						writeHeader();
					}
				}
			}
			else {
				Log::error("[{}] Connection::writeHeader - Write failed: {}", m_id, errorCode.message());
				m_socket.close();
			}
		}
	);
}

void Connection::writeBody() {
	asio::async_write(m_socket, asio::buffer(m_messagesOut.front().body.data(), m_messagesOut.front().body.size()),
		[this](std::error_code errorCode, std::size_t length) {
			if (!errorCode) {
				m_messagesOut.popFront();

				if (!m_messagesOut.isEmpty()) {
					writeHeader();
				}
			}
			else {
				Log::error("[{}] Connection::writeBody - Write failed: {}", m_id, errorCode.message());
				m_socket.close();
			}
		}
	);
}

void Connection::readHeader() {
	asio::async_read(m_socket, asio::buffer(&m_tempMessageIn.header, sizeof(Message_Header)),
		[this](std::error_code errorCode, std::size_t length) {
			if (!errorCode) {
				if (m_tempMessageIn.header.size > 0) {
					m_tempMessageIn.body.resize(m_tempMessageIn.header.size);
					readBody();
				}
				else {
					addToIncomingMessageQueue();
				}
			}
			else {
				Log::error("[{}] Connection::readHeader - Read failed: {}", m_id, errorCode.message());
				m_socket.close();
			}
		}
	);
}

void Connection::readBody() {
	asio::async_read(m_socket, asio::buffer(m_tempMessageIn.body.data(), m_tempMessageIn.body.size()),
		[this](std::error_code errorCode, std::size_t length) {
			if (!errorCode) {
				addToIncomingMessageQueue();
			}
			else {
				Log::error("[{}] Connection::readBody - Read failed: {}", m_id, errorCode.message());
				m_socket.close();
			}
		}
	);
}

void Connection::addToIncomingMessageQueue() {
	if (m_owner == Owner::SERVER) {
		m_messagesIn.pushBack({ this->shared_from_this(), m_tempMessageIn });
	}
	else {
		m_messagesIn.pushBack({ nullptr, m_tempMessageIn });
	}

	readHeader();
}


