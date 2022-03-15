import subprocess

#git describe --tags --abbrev=0 "main"
revision = (
    subprocess.check_output(["git", "describe", "--tags", "--abbrev=0", "main"])
    .strip()
    .decode("utf-8")
)
print("-DGIT_REV='\"%s\"'" % revision)
