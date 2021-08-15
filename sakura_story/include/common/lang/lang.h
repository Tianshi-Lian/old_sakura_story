#pragma once

#include "common/common.h"
#include "common/debug/instrumentor.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace sakura::common {

	enum class Language { EN, DE };

	constexpr u32 val_32_const = 0x811c9dc5;
	constexpr u32 prime_32_const = 0x1000193;

	inline uint32_t hash_32_fnv1a(const std::string& str) {
		const char* data = str.c_str();

		uint32_t hash = 0x811c9dc5;

		for (int i = 0; i < str.size(); ++i) {
			constexpr uint32_t prime = 0x1000193;
			const uint8_t value = data[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	constexpr uint32_t hash_32_fnv1a_const(const char* const str, const uint32_t value = val_32_const) noexcept {
		return (str[0] == '\0')
			? value
			: hash_32_fnv1a_const(&str[1], (value ^ static_cast<uint32_t>(str[0])) * prime_32_const);
	}

	constexpr u32 getLocalStringID(const char* data) { return hash_32_fnv1a_const(data); }

	static std::vector<std::string> splitStringDelimited(const std::string& str, char delimiter) {
		std::vector<std::string> result;
		std::stringstream stringStream(str);
		std::string item;
		while (std::getline(stringStream, item, delimiter)) {
			if (!item.empty()) {
				result.push_back(item);
			}
		}
		return result;
	}

	static void readLanguageFile(const std::string& filepath, std::unordered_map<u32, std::string>& out) {
		PROFILE_FUNCTION();
		Log::info("Loading language file: {0}", filepath);
		if (std::fstream file(filepath); file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {

				const std::string::size_type pos = line.find_first_of(' ');
				const std::string key = line.substr(0, pos);
				const std::string value = line.substr(pos + 1);
				// TODO(rob): Validate we found a space so we're not adding 1 to 0...

				u32 hash = hash_32_fnv1a(key);
				out[hash] = value;
			}
			file.close();
		}
		else {
			// TODO(rob): Logging!
		}
	}

}
