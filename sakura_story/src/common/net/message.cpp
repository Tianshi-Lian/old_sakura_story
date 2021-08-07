#include "common/net/message.h"

using namespace sakura::common::net;

u64 Message::size() const {
	return (sizeof(Message_Header) + body.size());
}
