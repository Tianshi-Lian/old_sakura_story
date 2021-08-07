#pragma once

#include "common/common.h"
#include "common/debug/logger.h"

static constexpr u32 NullMessageID = 0;

namespace sakura::common::net {

	struct Message_Header {
		u32 id = 0;
		u64 size = 0;
	};

	class Message {
	public:
		Message_Header header{};
		std::vector<u8> body;

		[[nodiscard]] u64 size() const;

		friend std::ostream& operator << (std::ostream& os, const Message& message) {
			os << "ID: " << message.header.id << " Size: " << message.header.size;
			return os;
		}

		// NOTE(rgg): This allows us to "pipe in" data into our message.
		template <typename type>
		friend Message& operator << (Message& message, const type& data) {
			static_assert(std::is_standard_layout<type>::value, "Data is too complex to be pushed into vector");

			if (message.header.id == NullMessageID) {
				Log::error("Message << - Message header is uninitialised.");
				return message;
			}

			const size_t position = message.body.size();

			message.body.resize(message.body.size() + sizeof(type));
			std::memcpy(message.body.data() + position, &data, sizeof(type));

			message.header.size = message.size();

			return message;
		}

		// NOTE(rgg): This allows us to "pipe out" data from our message.
		template <typename type>
		friend Message& operator >> (Message& message, type& data) {
			static_assert(std::is_standard_layout<type>::value, "Data is too complex to be pushed into vector");
			
			if (message.header.id == NullMessageID) {
				Log::error("Message << - Message header is uninitialised.");
				return message;
			}

			const size_t position = message.body.size() - sizeof(type);

			std::memcpy(&data, message.body.data() + position, sizeof(type));
			message.body.resize(position);

			message.header.size = message.size();

			return message;
		}
	};

	class Connection;

	class Owned_Message {
	public:
		std::shared_ptr<Connection> owner = nullptr;
		Message message{};

		friend std::ostream& operator << (std::ostream& os, const Owned_Message& self) {
			os << self.message;
			return os;
		}
	};
	
}
