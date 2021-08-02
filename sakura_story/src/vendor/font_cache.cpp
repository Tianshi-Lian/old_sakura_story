#include "vendor/font_cache.h"

// NOTE(rob): Compiler warnings from external libs be gone!
#pragma warning(push, 0)

// ReSharper disable CppUnusedIncludeDirective
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_gpu/SDL_gpu.h>

#include <SDL2_fc/SDL_FontCache.c>  // NOLINT(bugprone-suspicious-include)

#pragma warning(pop)
