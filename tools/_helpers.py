from collections import namedtuple

ParsedArgs = namedtuple("ParsedArgs", "command original dict")
Version = namedtuple("Version", "MAJOR MINOR PATCH BUILD")

def process_args(argv) -> ParsedArgs:
    dict = {} # key:value dictionary

    for arg in argv[1:]:
        #key || key=value
        try:
            k = arg[0:arg.index("=")]
            v = arg[arg.index("=") + 1:]
        except:
            k = arg
            v = 0
        dict[k] = v

    return ParsedArgs(argv[0], argv, dict)

def get_arg_value(args: ParsedArgs, names, default):
    for name in names:
        if name in args.dict:
            return args.dict[name]

    return default

def read_version(fileName) -> Version:
    V_MAJOR = 0
    V_MINOR = 0
    V_PATCH = 0
    V_BUILD = 0

    build_meta = open(fileName)
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

    return Version(V_MAJOR, V_MINOR, V_PATCH, V_BUILD)
