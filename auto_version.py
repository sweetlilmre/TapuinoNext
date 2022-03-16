import datetime

FILENAME_VERSION = 'version'
FILENAME_VERSION_H = 'include/Version.h'

prefix = '0.0.'
build_no = 0

try:
    with open(FILENAME_VERSION) as versionFile:
        build_no = versionFile.readline()
        prefix = build_no[0:build_no.rindex('.')+1]
        build_no = int(build_no[build_no.rindex('.')+1:])
        build_no = build_no + 1
except:
    build_no = 0

version = prefix + str(build_no)

with open(FILENAME_VERSION, 'w+') as versionFile:
    versionFile.write(version)


hf = """
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
with open(FILENAME_VERSION_H, 'w+') as f:
    f.write(hf)
