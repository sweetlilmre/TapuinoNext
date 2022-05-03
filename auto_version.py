import re
import datetime
import sys
import tapuino_common as tc

Import("env")

version_file_name = 'version'
header_file_name = 'include/Version.h'

build_no = 0
version = ''

print("********** AUTO VERSION **********")
cur_branch = tc.git_check("branch", "--no-color", "--show-current")
if cur_branch != "main":
    print("This script will only increment the version on the 'main' branch, you are on: " + cur_branch)
else:
    try:
        with open(version_file_name, newline='\n') as versionFile:
            version = versionFile.readline()

            p = re.compile(r"^(?P<version>(?P<major>0|[1-9]\d*)\.(?P<minor>0|[1-9]\d*)\.(?P<patch>0|[1-9]\d*)(?:-(?P<prerelease>(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?)(?:\+(?P<buildmetadata>0|[1-9]\d*))$")
            m = p.match(version)
            if m:
                build_no = int(m.group("buildmetadata")) + 1
                version = m.group("version") + "+" + str(build_no)
                try:
                    with open(version_file_name, 'w+', newline='\n') as versionFile:
                        versionFile.write(version)
                except:
                    tc.pretty_exit_error("Unable to increment version file!", file=sys.stderr)


            else:
              print ("Version file format is incorrect!", file=sys.stderr)
              tc.pretty_exit_error("Format must be: a.b.c-desc+build number", file=sys.stderr)

    except Exception as e:
        print(e)
        tc.pretty_exit_error("Version file: " + version_file_name + " is missing!", file=sys.stderr)

    header_string = """
    #ifndef FW_BUILD_NUMBER
      #define FW_BUILD_NUMBER "{}"
    #endif
    #ifndef FW_VERSION
      #define FW_VERSION "{}"
    #endif
    #ifndef FW_BUILD_TIME
      #define FW_BUILD_TIME "{}"
    #endif
    """.format(build_no, version, datetime.datetime.now())

    try:
        with open(header_file_name, 'w+', newline='\n') as header_file:
            header_file.write(header_string)
    except:
          tc.pretty_exit_error("Unable to generate version header!", file=sys.stderr)

    print("Building for version: " + version + ", build: " + str(build_no))
print("********** AUTO VERSION **********")