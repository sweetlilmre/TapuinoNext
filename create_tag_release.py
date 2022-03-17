import subprocess


try:
    branch = (
        subprocess.check_output(["git", "branch", "--no-color", "--show-current"])
        .strip()
        .decode("utf-8")
    )

    if branch != "main":
        print("This script must be run on the 'main' branch, you are on " + branch)
        exit()
    else:
        print("On 'main' branch, proceeding")


    FILENAME_VERSION = 'version'

    try:
        with open(FILENAME_VERSION) as versionFile:
            version = versionFile.readline().strip()
    except:
        print("Missing 'version' file! Exiting!\n")
        exit()


    subprocess.check_output(["git", "tag", version, "main"])
    subprocess.check_output(["git", "push", "origin", version])

except:
    print("Something went wrong here!\n")
