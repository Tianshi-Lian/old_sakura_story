#pragma once

#include "common/core/threadsafe_queue.h"
#include "common/net/message.h"
#include "common/net/net_common.h"

namespace sakura::common::net {

	class Connection : public std::enable_shared_from_this<Connection> {
	public:
		Connection(asio::io_context& context);
		virtual ~Connection() = default;

		bool connectToServer(asio::ip::tcp::resolver::results_type& endpoints);
		bool disconnect();
		bool isConnected() const;

		bool send(const Message& message);

	protected:
		asio::io_context& m_context;
		asio::ip::tcp::socket m_socket;
		
		core::Threadsafe_Queue<Message> m_messagesOut;
		core::Threadsafe_Queue<Owned_Message> m_messagesIn;
	};
	
}
