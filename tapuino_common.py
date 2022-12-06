import subprocess
import sys
import requests

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

def fetch_packer_script():
    file_url = "https://raw.githubusercontent.com/letscontrolit/ESPEasy/3678488f6a67ac9b899fab3d0ccd176c4437b1b4/tools/pio/post_esp32.py"
    response = requests.get(file_url)
    if response.status_code == 200:
        with open("post_esp32.py", "wb") as f:
            f.write(response.content)
        print("File downloaded successfully.")
    else:
        print("Failed to download file. Status code:", response.status_code)
