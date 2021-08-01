import _globals
import subprocess
import sys

exit_code = 0
if _globals.PLATFORM == "linux":
    process = subprocess.run(["vendor/premake/premake5", "gmake2"])
    exit_code = process.returncode
elif _globals.PLATFORM == "windows":
    process = subprocess.run(["vendor/premake/premake5", "vs2019"])
    exit_code = process.returncode

sys.exit(exit_code)
