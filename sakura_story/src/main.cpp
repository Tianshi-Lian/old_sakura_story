#include "types.h"

#include "debug/logger.h"
#include "debug/instrumentor.h"

int main(int argc, char *argv[]) {
    Log::initialize();
    Log::debug("Sakura Story initializing...");

    PROFILE_BEGIN_SESSION("Sakura Story", "logs/timings.json");

    // TODO(rob): work!

    PROFILE_END_SESSION();
    
    Log::debug("Sakura Story shutting down...");
    return (0);
}