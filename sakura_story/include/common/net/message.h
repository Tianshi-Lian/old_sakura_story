#pragma once

#include "common/common.h"

namespace sakura::common::net {

	struct Message_Header {
		u32 id{};
		u64 size = 0;
	};

	struct Message {
		Message_Header header{};
		std::vector<u8> body;

		[[nodiscard]] u64 size() const {
			return body.size();
		}

		friend std::ostream& operator << (std::ostream& os, const Message& msg) {
			os << "ID:" << static_cast<int>(msg.header.id) << " Size:" << msg.header.size;
			return os;
		}

		template<typename type>
		friend Message& operator << (Message& message, const type& data) {
			static_assert(std::is_standard_layout<type>::value, "Data is too complex to be pushed into vector");

			const size_t position = message.body.size();

			message.body.resize(message.body.size() + sizeof(type));

			std::memcpy(message.body.data() + position, &data, sizeof(type));

			message.header.size = message.size();

			return message;
		}

		template<typename type>
		friend Message& operator >> (Message& message, type& data) {
			static_assert(std::is_standard_layout<type>::value, "Data is too complex to be pulled from vector");

			const size_t position = message.body.size() - sizeof(type);

			std::memcpy(&data, message.body.data() + position, sizeof(type));

			message.body.resize(position);

			message.header.size = message.size();

			return message;
		}
	};

	class Connection;
	struct Owned_Message {
		std::shared_ptr<Connection> remote = nullptr;
		Message message;

		friend std::ostream& operator<<(std::ostream& os, const Owned_Message& oMessage) {
			os << oMessage.message;
			return os;
		}
	};

}
