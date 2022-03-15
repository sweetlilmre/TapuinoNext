import subprocess

try:
    revision = (
        subprocess.check_output(["git", "describe", "--tags", "--abbrev=0", "main"])
        .strip()
        .decode("utf-8")
    )
except:
    revision = "0.0.0"

print("-DGIT_REV='\"%s\"'" % revision)
