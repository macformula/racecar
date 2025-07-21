Import("env")

import subprocess

def generate():
    subprocess.run(["cangen"])

env.AddPreAction("build", generate)
