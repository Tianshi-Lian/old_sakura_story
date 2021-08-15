#pragma once

#include "common/common.h"

#include "common/core/threadsafe_queue.h"

#include "common/net/common.h"
#include "common/net/connection.h"
#include "common/net/message.h"

namespace sakura::client::net {

	class Client {
	public:
		Client() = default;
		virtual ~Client();

		bool connect(const std::string& host, uint16_t port);
		void disconnect();

		void send(const common::net::Message& msg) const;

		void setId(u32 id) const;

		[[nodiscard]] bool isConnected() const;
		[[nodiscard]] u32 getId() const;
		[[nodiscard]] common::core::Threadsafe_Queue<common::net::Owned_Message>& incoming();

	protected:
		asio::io_context m_context;
		std::thread m_contextThread;

		std::unique_ptr<common::net::Connection> m_connection;

	private:
		common::core::Threadsafe_Queue<common::net::Owned_Message> m_messagesIn;
	};

}
