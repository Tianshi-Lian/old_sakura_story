#pragma once

#include "common/types.h"

#include <string>

namespace sakura::client::graphics {

	class Font {
	public:
		Font();
		~Font();

		void load(const std::string& filepath, u32 pointSize/*, SDL_Color color, int style*/);

		void setSize(u32 pointSize);
		[[nodiscard]] u32 getSize() const;

		[[nodiscard]] void* getInternal() const;

	private:
		void* m_internal;

		std::string m_filepath;

		u32 m_pointSize;
	};

}