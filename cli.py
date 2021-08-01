# CLI tool for handling project_s build pipeline
# cli help      - displays a list of CLI commands
# cli gen       - gens project for active platform
# cli version   - returns current project version
# cli build     - builds project for active platform
# cli run       - runs project for current platform
# cli clean     - cleans all project files, build files and build code
# Commands may be chained together e.g: cli clean gen build run

import sys, os
import subprocess

SCRIPTS_DIR = "tools"
SUCCESS = 0

def run_command(cmd: str):
    exit_code = 0

    # As we just run any file specified by cmd we should at least check it's not one _we_ are ignoring
    if cmd.startswith("_"):
        print("Invalid CLI command! Type `cli help` for a list of commands.")
        exit_code = -1
    else:
        script = "./{}/{}.py".format(SCRIPTS_DIR, cmd)
        print("==============================================")

        if os.path.exists(script):
            print("Executing {}:".format(cmd))
            process = subprocess.run(["python", script])
            exit_code = process.returncode
        else:
            print("Invalid CLI command! Type `cli help` for a list of commands.")
            exit_code = -1
    
    return exit_code


argc = len(sys.argv)
if argc < 2:
    print("Invalid CLI command! Type `cli help` for a list of commands.")
else:
    # TODO: parse parameters for a command. e.g: cli build --ver=windows

    for i in range(1, len(sys.argv)):
        cmd = sys.argv[i]

        if (run_command(cmd) != SUCCESS):
            break; # this will stop us running any more commands if we failed one