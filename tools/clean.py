import os, sys
import shutil


def clean_directory(dirname):
    for roots, dirs, files in os.walk(dirname):
        for folder in dirs:
            clean_directory(os.path.join(dirname, folder))

        for name in files:
            if name.endswith(".sln") or ".vcxproj" in name or name == "Makefile":
                filename = os.path.join(dirname, name)
                print("Removing: {}...".format(filename))
                os.remove(filename)

exit_code = 0
try:
    print("Removing: ./.vs/*...")
    shutil.rmtree(".vs", ignore_errors=True)
    print("Removing: ./.vscode/*...")
    shutil.rmtree(".vscode", ignore_errors=True)
    print("Removing: ./bin/*...")
    shutil.rmtree("bin", ignore_errors=True)
    print("Removing: ./bin-int/*...")
    shutil.rmtree("bin-int", ignore_errors=True)
    print("Removing: ./tools/__pycache__/*...")
    shutil.rmtree("./tools/__pycache__", ignore_errors=True)

    clean_directory(".")

except OSError as e:
    exit_code = -1

sys.exit(exit_code)
