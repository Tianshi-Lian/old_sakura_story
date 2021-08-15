import _globals
import sys

print("{} build - v{}.{}.{}.{}".format(_globals.BUILD_PRJ_NAME, _globals.V_MAJOR, _globals.V_MINOR, _globals.V_PATCH, _globals.V_BUILD))

sys.exit(0)
