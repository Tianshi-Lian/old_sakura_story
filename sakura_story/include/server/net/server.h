#pragma once

#include "common/common.h"
#include "common/net/connection.h"

namespace sakura::server::net {

	class Server {
	public:
		Server(uint16_t port);
		virtual ~Server();

		bool start();
		void stop();

		void waitForClientConnection();

		void messageClient(std::shared_ptr<common::net::Connection> client, const common::net::Message& message);
		void messageAllClients(const common::net::Message& message, const std::shared_ptr<common::net::Connection>& ignoredClient = nullptr);

		void processMessages(u64 maxMessages = -1, bool wait = false);

	protected:
		virtual bool onClientConnect(std::shared_ptr<common::net::Connection> client);
		virtual void onClientDisconnect(std::shared_ptr<common::net::Connection> client);
		virtual void onMessage(std::shared_ptr<common::net::Connection> client, common::net::Message& message);

		common::core::Threadsafe_Queue<common::net::Owned_Message> m_MessagesIn;

		std::deque<std::shared_ptr<common::net::Connection>> m_connections;

		asio::io_context m_context;
		std::thread m_contextThread;

		asio::ip::tcp::acceptor m_acceptor;

		uint32_t nIDCounter = 10000;
	};

}
