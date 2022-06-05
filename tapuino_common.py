import subprocess
import sys

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

def pretty_exit_error(message):
    print(message, file=sys.stderr)
    exit(1)

def pretty_exit_ok(message):
    print(message)
    exit(0)
