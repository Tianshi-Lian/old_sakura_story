import _globals
import _helpers
import subprocess
import sys


args = _helpers.process_args(sys.argv)
project = _helpers.get_arg_value(args, ["project", "p"], _globals.BUILD_PRJ_NAME)
config = _helpers.get_arg_value(args, ["config", "c"], _globals.BUILD_CFG)
platform = _globals.PLATFORM

exit_code = 0
workingDir = "bin/{}-{}-x86_64/{}".format(platform, config, project)
if _globals.PLATFORM == "windows":
    process = subprocess.run(["{}/{}".format(workingDir, project)], cwd=workingDir)
    exit_code = process.returncode
elif _globals.PLATFORM == "linux":
    process = subprocess.run(["./{}".format(project)], cwd=workingDir)
    exit_code = process.returncode

sys.exit(exit_code)
