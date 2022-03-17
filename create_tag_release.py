import subprocess



def git_check(command, *args):
    arg_list = ["git", command]
    for arg in args:
        arg_list.append(arg)

    result = (
        subprocess.check_output(arg_list)
        .strip()
        .decode("utf-8")
    )
    return result


try:
    if git_check("branch", "--no-color", "--show-current") != "main":
        print("This script must be run on the 'main' branch, you are on " + branch)
        exit()

    print("On 'main' branch, proceeding")

    changes_pending = git_check("status", "--porcelain")
    if changes_pending != "":
        print("You have the following unstaged or uncommitted files:\n" + changes_pending)
        exit()

    not_synced = git_check("cherry", "-v")
    if not_synced != "":
        print("The following commits have not been pushed to main:\n" + not_synced)
        exit()

    print("Everything seems to be ready for a release, let's go!")


    FILENAME_VERSION = 'version'

    try:
        with open(FILENAME_VERSION) as versionFile:
            version = versionFile.readline().strip()
    except:
        print("Missing 'version' file! Exiting!\n")
        exit()


    subprocess.check_output(["git", "tag", version, "main"])
    subprocess.check_output(["git", "push", "origin", version])

except Exception as e:
    print("Something went wrong here!\n")
    print(e)
