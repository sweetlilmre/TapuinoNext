import subprocess

version_file_name = 'version'

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

def get_confirmation(question):
    answer = input(question + "(y/N): ")
    if answer.upper() == "Y":
        return True
    return False

def pretty_exit(message):
    print(message)
    exit(1)

try:
    if git_check("branch", "--no-color", "--show-current") != "main":
        print("This script must be run on the 'main' branch, you are on " + branch)
        exit(1)

    print("On the 'main' branch, proceeding...")

    while True:
        changes_pending = git_check("status", "--porcelain")
        if changes_pending == "":
            print("No pending changes, proceeding...");
            break

        print("You have the following unstaged or uncommitted files:\n" + changes_pending)
        if get_confirmation("Would you like to commit these changes?"):
            git_check("commit", "-a", "-v")
        else:
            pretty_exit("Quitting due to pending changes")

    while True:
        not_synced = git_check("cherry", "-v")
        if not_synced == "":
            print("No pending commits, proceeding...");
            break

        print("The following commits have not been pushed to main:\n" + not_synced)
        if get_confirmation("Would you like to push these commits?"):
            git_check("push", "origin", "main")
        else:
            pretty_exit("Quitting due to pending commits")

    print("Everything seems to be ready for a release, let's go!")

    try:
        with open(version_file_name) as versionFile:
            version = versionFile.readline().strip()
    except:
        print("Missing 'version' file! Exiting!\n")
        exit(1)

    subprocess.check_output(["git", "tag", version, "main"])
    subprocess.check_output(["git", "push", "origin", version])

except Exception as e:
    print("Something went wrong here!\n")
    print(e)
