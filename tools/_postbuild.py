import _globals
import _helpers
import sys

args = _helpers.process_args(sys.argv)
project = _helpers.get_arg_value(args, ["project", "p"], _globals.BUILD_PRJ_NAME)

def run_postbuild(pname):
    metaDataFile = "{}.build.meta".format(pname, pname)
    print(metaDataFile)
    currentVersion = _helpers.read_version(metaDataFile)
    build_meta = open(metaDataFile, 'w')
    versioning_meta = ["MAJOR {}\n".format(currentVersion.MAJOR), "MINOR {}\n".format(currentVersion.MINOR), "PATCH {}\n".format(currentVersion.PATCH), "BUILD {}".format(currentVersion.BUILD + 1)]
    build_meta.writelines(versioning_meta)
    build_meta.close()

run_postbuild(project)