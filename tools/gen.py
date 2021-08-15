import _globals
import _helpers
import subprocess
import sys


args = _helpers.process_args(sys.argv)
platform = _helpers.get_arg_value(args, ["sys", "s"], _globals.PLATFORM)

exit_code = 0
if platform == "linux":
    process = subprocess.run(["vendor/premake/premake5", "gmake2"])
    exit_code = process.returncode
elif platform == "windows":
    process = subprocess.run(["vendor/premake/premake5", "vs2019"])
    exit_code = process.returncode
else:
    print("Unknown platform: {}. Please use one of: 'linux', 'windows'.".format(platform))
    exit_code = -1

sys.exit(exit_code)
