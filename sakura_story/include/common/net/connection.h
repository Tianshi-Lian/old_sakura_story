#pragma once

#include "common/core/threadsafe_queue.h"

#include "common/net/common.h"
#include "common/net/message.h"

namespace sakura::server::net {
	class Server;
}

namespace sakura::common::net {

	class Connection : public std::enable_shared_from_this<Connection> {
	public:
		enum class Owner {
			SERVER,
			CLIENT
		};

	public:
		Connection(Owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, core::Threadsafe_Queue<Owned_Message>& queueIn);

		virtual ~Connection() = default;


		void connectToClient(server::net::Server* server, uint32_t uid = 0);
		void connectToServer(const asio::ip::tcp::resolver::results_type& endpoints);

		void disconnect();

		void send(const Message& msg);
		void startListening();

		void setId(u32 id);

		[[nodiscard]] u32 getId() const;
		[[nodiscard]] bool isConnected() const;

	private:
		void writeValidation();
		void writeHeader();
		void writeBody();

		void readValidation(server::net::Server* server = nullptr);
		void readHeader();
		void readBody();

		void addToIncomingMessageQueue();

		[[nodiscard]] u64 scrambleData(u64 input) const;

	protected:
		asio::ip::tcp::socket m_socket;
		asio::io_context& m_context;

		core::Threadsafe_Queue<Message> m_messagesOut;

		core::Threadsafe_Queue<Owned_Message>& m_messagesIn;

		Message m_tempMessageIn;

		Owner m_owner = Owner::SERVER;

		u32 m_id = 0;

		u64 m_handshakeOut = 0;
		u64 m_handshakeIn = 0;
		u64 m_handshakeCheck = 0;

	};
}
