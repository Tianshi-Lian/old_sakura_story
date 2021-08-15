import _globals
import _helpers
import subprocess
import sys


args = _helpers.process_args(sys.argv)
config = _helpers.get_arg_value(args, ["config", "c"], _globals.BUILD_CFG)

exit_code = 0
if _globals.PLATFORM == "linux":
    process = subprocess.run(["make", "config={}".format(config.lower())]) # make uses lowercase config regardless of what is in premake5
    exit_code = process.returncode
elif _globals.PLATFORM == "windows":
    process = subprocess.run(["msbuild", "/property:Configuration={}".format(config)])
    exit_code = process.returncode

build_meta = open("build.meta", 'w')
versioning_meta = ["MAJOR {}\n".format(_globals.V_MAJOR), "MINOR {}\n".format(_globals.V_MINOR), "PATCH {}\n".format(_globals.V_PATCH), "BUILD {}".format(_globals.V_BUILD + 1)]
build_meta.writelines(versioning_meta)

sys.exit(exit_code)
