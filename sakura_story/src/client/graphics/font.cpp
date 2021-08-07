#include "client/graphics/font.h"

#include "common/debug/logger.h"
#include "common/debug/instrumentor.h"

#include <SDL2_fc/SDL_FontCache.h>


using namespace sakura::client::graphics;

Font::Font() :
	m_internal(nullptr),
	m_pointSize(0) {
}

Font::~Font() {
	if (m_internal) {
		Log::info("Destroying font: {} size: {}", m_filepath, m_pointSize);
		FC_FreeFont(m_internal);
	}
}

void Font::load(const std::string& filepath, u32 pointSize) {
	Log::info("Loading font: {} size: {}", filepath, pointSize);
	PROFILE_FUNCTION()

	m_internal = FC_CreateFont();
	FC_LoadFont(m_internal, filepath.c_str(), pointSize, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	m_filepath = filepath;
	m_pointSize = pointSize;
}

void Font::setSize(u32 pointSize) {
	PROFILE_FUNCTION()
	FC_FreeFont(m_internal);

	m_internal = FC_CreateFont();
	FC_LoadFont(m_internal, m_filepath.c_str(), pointSize, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	m_pointSize = pointSize;
}

u32 Font::getSize() const {
	return m_pointSize;
}

void* Font::getInternal() const {
	return m_internal;
}
