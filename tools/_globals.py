import sys

V_MAJOR: int = 0
V_MINOR: int = 0
V_PATCH: int = 0
V_BUILD: int = 0

BUILD_CFG = "debug"
BUILD_PRJ_NAME = "sakura_story"

PLATFORM = sys.platform

if 'linux' in PLATFORM: # we could be linux or linux2
    PLATFORM = 'linux'
elif 'win' in PLATFORM:
    PLATFORM = 'windows'

build_meta = open("build.meta")
version_lines = build_meta.readlines()
for line in version_lines:
    # TODO: Possibly have a way to check for a version upgrade and reset "smaller" values?
    front, back = line.split()
    if front == "MAJOR":
        V_MAJOR = int(back)
    elif front == "MINOR":
        V_MINOR = int(back)
    elif front == "PATCH":
        V_PATCH = int(back)
    elif front == "BUILD":
        V_BUILD = int(back)
build_meta.close()