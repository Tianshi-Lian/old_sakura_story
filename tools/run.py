import _globals
import subprocess
import sys

exit_code = 0
workingDir = "bin/{}-{}-x86_64/{}".format(_globals.PLATFORM, _globals.BUILD_CFG, _globals.BUILD_PRJ_NAME)
if _globals.PLATFORM == "windows":
    process = subprocess.run(["{}/{}".format(workingDir, _globals.BUILD_PRJ_NAME)], cwd=workingDir)
    exit_code = process.returncode
elif _globals.PLATFORM == "linux":
    process = subprocess.run(["./{}".format(_globals.BUILD_PRJ_NAME)], cwd=workingDir)
    exit_code = process.returncode

sys.exit(exit_code)
