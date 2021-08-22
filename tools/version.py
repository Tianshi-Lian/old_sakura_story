import _globals
import _helpers
import sys


args = _helpers.process_args(sys.argv)
project = _helpers.get_arg_value(args, ["project", "p"], _globals.BUILD_PRJ_NAME)

version = _helpers.read_version("{}/{}.build.meta".format(project, project))
print("{} build - v{}.{}.{}.{}".format(project, version.MAJOR, version.MINOR, version.PATCH, version.BUILD))

sys.exit(0)
