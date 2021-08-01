#include "types.h"

#include "debug/logger.h"
#include "debug/instrumentor.h"

int main(int argc, char *argv[]) {
    Log::initialize();
    Log::debug("Project initializing...");

    PROFILE_BEGIN_SESSION("project", "logs/timings.json");

    // TODO(rob): work!

    PROFILE_END_SESSION();
    
    Log::debug("Project shutting down...");
    return (0);
}