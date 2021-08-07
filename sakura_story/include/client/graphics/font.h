#pragma once

#include "common/common.h"

#include <string>

struct FC_Font;

namespace sakura::client::graphics {

	class Font {
	public:
		Font();
		~Font();

		void load(const std::string& filepath, u32 pointSize);

		void setSize(u32 pointSize);
		[[nodiscard]] u32 getSize() const;

		[[nodiscard]] void* getInternal() const;

	private:
		FC_Font* m_internal;

		std::string m_filepath;

		u32 m_pointSize;
	};

}