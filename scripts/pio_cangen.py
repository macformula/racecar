Import("env")

import subprocess

subprocess.check_call(["cangen", "."])
