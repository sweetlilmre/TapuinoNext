import re
import datetime
import sys

Import("env")

version_file_name = 'version'
header_file_name = 'include/Version.h'

build_no = 0;
version = ''

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
                print ("Unable to increment version file!", file=sys.stderr)
                env.Exit(1)


        else:
          print ("Version file format is incorrect!", file=sys.stderr)
          print ("Format must be: a.b.c-desc+build number", file=sys.stderr)
          env.Exit(1)

except Exception as e:
    print(e)
    print ("Version file: " + version_file_name + " is missing!", file=sys.stderr)
    env.Exit(1)

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
      print ("Unable to generate version header!", file=sys.stderr)
      env.Exit(1)

print("Building for version: " + version + ", build: " + str(build_no))
