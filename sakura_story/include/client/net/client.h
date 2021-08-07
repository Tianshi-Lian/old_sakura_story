#pragma once

#include "common/common.h"
#include "common/core/threadsafe_queue.h"
#include "common/net/net_common.h"
#include "common/net/message.h"

namespace sakura::client::net {

	class Client {
	public:
		Client();
		virtual ~Client();
		
		bool connect(const std::string& host, u16 port);
		bool disconnect();
		[[nodiscard]] bool isConnected() const;

		common::core::Threadsafe_Queue<common::net::Owned_Message>& incoming();

	protected:
		asio::io_context m_context;
		std::thread m_contextThread;

		asio::ip::tcp::socket m_socket;
		
		std::unique_ptr<common::net::Connection> m_connection;
	
	private:
		common::core::Threadsafe_Queue<common::net::Owned_Message> m_messagesIn;
	};
	
}
