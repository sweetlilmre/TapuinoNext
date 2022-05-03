import subprocess
import tapuino_common as tc

version_file_name = 'version'

try:
    cur_branch = tc.git_check("branch", "--no-color", "--show-current")
    if cur_branch != "main":
        tc.pretty_exit_error("This script must be run on the 'main' branch, you are on: " + cur_branch)
        

    print("On the 'main' branch, proceeding...")

    while True:
        changes_pending = tc.git_check("status", "--porcelain")
        if changes_pending == "":
            print("No pending changes, proceeding...")
            break

        print("You have the following unstaged or uncommitted files:\n" + changes_pending)
        if tc.get_confirmation("Would you like to commit these changes?"):
            tc.git_check("commit", "-a", "-v")
        else:
            tc.pretty_exit_error("Quitting due to pending changes")

    while True:
        not_synced = tc.git_check("cherry", "-v")
        if not_synced == "":
            print("No pending commits, proceeding...")
            break

        print("The following commits have not been pushed to main:\n" + not_synced)
        if tc.get_confirmation("Would you like to push these commits?"):
            tc.git_check("push", "origin", "main")
        else:
            tc.pretty_exit_error("Quitting due to pending commits")

    print("Everything seems to be ready for a release, let's go!")

    try:
        with open(version_file_name) as versionFile:
            version = versionFile.readline().strip()
    except:
        tc.pretty_exit_error("Missing 'version' file! Exiting!\n")

    subprocess.check_output(["git", "tag", version, "main"])
    subprocess.check_output(["git", "push", "origin", version])

except Exception as e:
    print("Something went wrong here!\n")
    tc.pretty_exit_error(e)
